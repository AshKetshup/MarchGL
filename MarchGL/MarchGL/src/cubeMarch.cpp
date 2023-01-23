#include "cubeMarch.h"

cubeMarch::cubeMarch(unsigned w, unsigned h, std::string obj) {
	this->obj = obj;

	if (obj == "torus")
		box_lim = glm::vec3(R1 + R2, R1 + R2, R2);

	if (obj == "sphere")
		box_lim = glm::vec3(radius);

	std::cout << "LOADING CubeMarch SHADER: ... " << std::endl << std::endl;
	shader = Shader("shaders/basicShader_vs.glsl", "shaders/basicShader_fs.glsl");
	if (!shader.wasSuccessful()) {
		std::cout << "Shader was not successful" << std::endl;
		std::cout << shader.getReport() << std::endl;
	} else
		std::cout << std::endl << "[DONE]" << std::endl;


	createSphere();
	createGrid();
	marchingCubesSimple();
	createMesh();
}

//----sphere (for comparison)----
void cubeMarch::createSphere() {
	std::cout << "Create Sphere: ... ";

	unsigned VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	//----------------------------------------sphere---------------------------------------
	// sphere formula = x^2 + y^2 + z^2 = r^2
	// (=) z = sqrt(r^2 - x^2 - y^2)

	//2y(y^{2}-3x^{2})(1-z^{2})+(x^{2}+y^{2})^{2}-(9z^{2}-1)(1-z^{2})=0
	float radius = 1.0f;
	for (float x = -1; x < 1; x += 0.001) {
		for (float y = -1; y < 1; y += 0.001) {
			vertices.push_back(x);
			vertices.push_back(y);
			float z = sqrt(radius * radius - x * x - y * y);
			vertices.push_back(z);

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(-z);
		}
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	std::cout << "[DONE]" << std::endl;
}

void cubeMarch::drawSphere(Camera camera) {
	std::cout << "Draw Sphere: ... ";

	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;

	projection = glm::perspective(glm::radians(camera.Zoom), (float) width / (float) height, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	model = glm::mat4(1.0f);
	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);

	glBindVertexArray(VAO);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(10.0);
	glDrawArrays(GL_POINTS, 0, vertices.size() * 1);

	std::cout << "[DONE]" << std::endl;
}



//----grid----
void cubeMarch::createGrid() {
	std::cout << "Create Grid: ... ";

	for (float x = -( box_lim[0] ); x <= box_lim[0]; x += gridDist) {
		for (float y = -box_lim[1]; y <= box_lim[1]; y += gridDist) {
			for (float z = -box_lim[2]; z <= box_lim[2]; z += gridDist) {
				//for marching cubes 
				VOXEL tmp;
				tmp.p0 = glm::vec3(x, y, z);
				tmp.p1 = glm::vec3(x + gridDist, y, z);
				tmp.p2 = glm::vec3(x + gridDist, y + gridDist, z);
				tmp.p3 = glm::vec3(x, y + gridDist, z);
				tmp.p4 = glm::vec3(x, y, z + gridDist);
				tmp.p5 = glm::vec3(x + gridDist, y, z + gridDist);
				tmp.p6 = glm::vec3(x + gridDist, y + gridDist, z + gridDist);
				tmp.p7 = glm::vec3(x, y + gridDist, z + gridDist);
				voxels.push_back(tmp);

				//for drawing
				cmgrid.push_back(x);
				cmgrid.push_back(y);
				cmgrid.push_back(z);
				cmgrid.push_back(0.9f);
				cmgrid.push_back(0.58f);
				cmgrid.push_back(0.47f);
			}
		}
	}

	gridPoints = cmgrid.size() / 6;
	unsigned VBO;
	glGenVertexArrays(1, &gridVAO);
	glGenBuffers(1, &VBO);


	glBindVertexArray(gridVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, cmgrid.size() * sizeof(float), &cmgrid[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) ( 3 * sizeof(float) ));

	std::cout << "[DONE]" << std::endl;
}

void cubeMarch::drawGrid(Camera camera) {
	//std::cout << "Drawing Grid: ...";

	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;

	projection = glm::perspective(glm::radians(camera.Zoom), (float) 1920 / (float) 1080, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	model = glm::mat4(1.0f);
	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);

	glBindVertexArray(gridVAO);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(20.0);
	glDrawArrays(GL_POINTS, 0, gridPoints);

	//std::cout << "[DONE]" << std::endl;
}



//-----marching cubes algorithm-----

//cheks if the point is inside or outside the sphere
bool cubeMarch::checkIsosurface(glm::vec3 p) {
	if (obj == "torus") {
		//x = (R1 + R2*cos(v)) * cos(u)
		//y = (R1 + R2*cos(v)) * sin(u)
		//z = R2 * sin(v)
		//(sqrt(x^2+y^2) - R)^2 + z^2 = r^2

		return ( pow(sqrt(p.x * p.x + p.y * p.y) - R1, 2) + p.z * p.z - R2 * R2 <= 0 );
	}

	//----default----
	//x^2 + y^2 + z^2 = r^2 (sphere)
	return ( p.x * p.x + p.y * p.y + p.z * p.z - radius * radius <= 0 );
}

//returns the middle point between the two vertices
glm::vec3 cubeMarch::getIntersVertice(glm::vec3 p1, glm::vec3 p2) {
	return glm::vec3(( p1.x + p2.x ) / 2, ( p1.y + p2.y ) / 2, ( p1.z + p2.z ) / 2);
}

//marching cubes without isovalues
void cubeMarch::marchingCubesSimple() {
	std::cout << "Marching Cubes Simple: ... ";

	for (int i = 0; i < voxels.size(); i++) {
		//check what vertice is inside the sphere
		string bin = "00000000";

		if (checkIsosurface(voxels[i].p0)) bin[7] = '1';
		if (checkIsosurface(voxels[i].p1)) bin[6] = '1';
		if (checkIsosurface(voxels[i].p2)) bin[5] = '1';
		if (checkIsosurface(voxels[i].p3)) bin[4] = '1';
		if (checkIsosurface(voxels[i].p4)) bin[3] = '1';
		if (checkIsosurface(voxels[i].p5)) bin[2] = '1';
		if (checkIsosurface(voxels[i].p6)) bin[1] = '1';
		if (checkIsosurface(voxels[i].p7)) bin[0] = '1';

		//cube either doesn't have any vertice in the shape, or as all of them
		if (bin == "00000000" || bin == "11111111")
			continue;

		//case number (conversion from binary to decimal)
		int case_n = std::stoi(bin, 0, 2);

		//edges that intersects the isosurface (binary)
		int edgesInters = tbl::edgeTable[case_n];

		//get all the vertices for the triangles
		glm::vec3 vertices[12];

		//no edges intersect the isosurface
		if (edgesInters == 0)
			continue;

		// (edgesInters & 1) -> verifies if the the right most bit of edgesInters is 1
		// this allows to check if the edge 0 is in the edges intersected by the implicit function
		// f.e: edgesInters = 0011 -> true
		//      edgesInters = 0100 -> false
		if (edgesInters & 1) vertices[0] = ( getIntersVertice(voxels[i].p0, voxels[i].p1) ); //edge 0
		if (edgesInters & 2) vertices[1] = ( getIntersVertice(voxels[i].p1, voxels[i].p2) ); //edge 1
		if (edgesInters & 4) vertices[2] = ( getIntersVertice(voxels[i].p2, voxels[i].p3) ); //edge 2
		if (edgesInters & 8) vertices[3] = ( getIntersVertice(voxels[i].p3, voxels[i].p4) ); //edge 3 
		if (edgesInters & 16) vertices[4] = ( getIntersVertice(voxels[i].p4, voxels[i].p5) ); //edge 4
		if (edgesInters & 32) vertices[5] = ( getIntersVertice(voxels[i].p5, voxels[i].p6) ); //edge 5
		if (edgesInters & 64) vertices[6] = ( getIntersVertice(voxels[i].p6, voxels[i].p7) ); //edge 6
		if (edgesInters & 128) vertices[7] = ( getIntersVertice(voxels[i].p4, voxels[i].p7) ); //edge 7
		if (edgesInters & 256) vertices[8] = ( getIntersVertice(voxels[i].p0, voxels[i].p4) ); //edge 8
		if (edgesInters & 512) vertices[9] = ( getIntersVertice(voxels[i].p1, voxels[i].p5) ); //edge 9
		if (edgesInters & 1024) vertices[10] = ( getIntersVertice(voxels[i].p2, voxels[i].p6) ); //edge 10
		if (edgesInters & 2048) vertices[11] = ( getIntersVertice(voxels[i].p3, voxels[i].p7) ); //edge 11


		// Adds the triangles vertices in the correct order to the meshTriangles list
		for (int n = 0; tbl::triTable[case_n][n] != -1; n += 3) {
			meshTriangles.push_back(vertices[tbl::triTable[case_n][n]]);
			meshTriangles.push_back(vertices[tbl::triTable[case_n][n + 1]]);
			meshTriangles.push_back(vertices[tbl::triTable[case_n][n + 2]]);
		}

	}

	std::cout << "[DONE]" << std::endl;
}

//----mesh----
void cubeMarch::createMesh() {
	std::cout << "Create Mesh: ... ";

	std::vector<glm::vec3> colors;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);
	for (int i = 0; i < meshTriangles.size(); i += 3) {
		colors.push_back(glm::vec3(dist(gen), dist(gen), dist(gen)));
		colors.push_back(glm::vec3(dist(gen), dist(gen), dist(gen)));
		colors.push_back(glm::vec3(dist(gen), dist(gen), dist(gen)));
	}

	unsigned VBO, CBO;
	glGenVertexArrays(1, &meshVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &CBO);
	glBindVertexArray(meshVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, meshTriangles.size() * sizeof(glm::vec3), &meshTriangles[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

	glBindVertexArray(0);

	std::cout << "[DONE]" << std::endl;
}

void cubeMarch::drawMesh(Camera camera, glm::vec3 trans, SHADER_SETTINGS& settings) {
	//std::cout << "Draw Mesh: ...";

	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;

	projection = glm::perspective(glm::radians(camera.Zoom), (float) 1920 / (float) 1080, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	model = glm::mat4(1.0f);
	model = glm::translate(model, trans);
	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);

	shader.setVec4("objectColor", settings.colorMesh);

	shader.setVec4("lamp.lightColor", settings.colorLight);
	shader.setVec3("lamp.lightPos", ( settings.cameraLightSnap ) ? camera.Position : settings.lightPos);
	shader.setVec3("lamp.viewPos", camera.Position);

	glBindVertexArray(meshVAO);
	glDrawArrays(GL_TRIANGLES, 0, meshTriangles.size());

	//std::cout << "[DONE]" << std::endl;
}