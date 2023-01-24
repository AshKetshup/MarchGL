#include "cubeMarch.h"

#include <glm/gtx/normal.hpp>

cubeMarch::cubeMarch(std::string obj) {
	this->obj = obj;

	if (obj == "torus")
		box_lim = glm::vec3(R1 + R2, R1 + R2, R2);

	if (obj == "sphere")
		box_lim = glm::vec3(radius+1);

	std::cout << "LOADING CubeMarch SHADER: ... " << std::endl << std::endl;
	shader = Shader("shaders/basicShader_vs.glsl", "shaders/basicShader_fs.glsl");
	if (!shader.wasSuccessful()) {
		std::cout << "Shader was not successful" << std::endl;
		std::cout << shader.getReport() << std::endl;
	} else
		std::cout << std::endl << "[DONE]" << std::endl;

	basicShader = Shader("shaders/basicColorShader.vs", "shaders/basicColorShader.fs");


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
			cmgrid.push_back(0.9f);
			cmgrid.push_back(0.58f);
			cmgrid.push_back(0.47f);


			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(-z);
			cmgrid.push_back(0.9f);
			cmgrid.push_back(0.58f);
			cmgrid.push_back(0.47f);

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

	projection = glm::perspective(glm::radians(camera.Zoom), (float)1920 / (float) 1080, 0.1f, 100.0f);
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
				
				tmp.p[0] = glm::vec3(x, y, z);
				tmp.p[1] = glm::vec3(x + gridDist, y, z);
				tmp.p[2] = glm::vec3(x + gridDist, y + gridDist, z);
				tmp.p[3] = glm::vec3(x, y + gridDist, z);
				tmp.p[4] = glm::vec3(x, y, z + gridDist);
				tmp.p[5] = glm::vec3(x + gridDist, y, z + gridDist);
				tmp.p[6] = glm::vec3(x + gridDist, y + gridDist, z + gridDist);
				tmp.p[7] = glm::vec3(x, y + gridDist, z + gridDist);
				
				voxels.push_back(tmp);

				//for drawing
				//cmgrid.push_back(x);
				//cmgrid.push_back(y);
				//cmgrid.push_back(z);
				//cmgrid.push_back(1.0f);
				//cmgrid.push_back(0.6f);
				//cmgrid.push_back(0.47f);
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
	basicShader.use();
	basicShader.setMat4("projection", projection);
	basicShader.setMat4("view", view);
	basicShader.setMat4("model", model);

	glBindVertexArray(gridVAO);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 0, gridPoints);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//std::cout << "[DONE]" << std::endl;
}



//-----marching cubes algorithm-----


//returns the density of the point p
float cubeMarch::getDensity(glm::vec3 p) {
	if (obj == "torus") {
		//x = (R1 + R2*cos(v)) * cos(u)
		//y = (R1 + R2*cos(v)) * sin(u)
		//z = R2 * sin(v)
		//(sqrt(x^2+y^2) - R)^2 + z^2 = r^2

		return ( pow( sqrt(p.x * p.x + p.y * p.y) - R1, 2) + p.z * p.z - R2 * R2);
	}

	//----default----
	//x^2 + y^2 + z^2 = r^2 (sphere)
	return ( p.x * p.x + p.y * p.y + p.z * p.z - radius * radius);
}

//returns the middle point between the two vertices
glm::vec3 cubeMarch::getIntersVertice(glm::vec3 p1, glm::vec3 p2, float D1, float D2) {
	if (abs(D1) < 0.00001) return p1;
	if (abs(D2) < 0.00001) return p2;
	if (abs(D1 - D2) < 0.00001) return p1;
	float t = -D1 / (D2 - D1);
	/*
	printf("---------------------LI----------------------\n");
	printf("Point 1: %f %f %f\n", p1.x, p1.y, p1.z);
	printf("Point 2: %f %f %f\n", p2.x, p2.y, p2.z);
	printf("D1: %f\n", D1); 
	printf("D2: %f\n", D2);
	printf("t: %f\n", t);
	printf("Inter point: %f %f %f\n", (1 - t) * p1.x + t * p2.x, (1 - t) * p1.y + t * p2.y, (1 - t) * p1.z + t * p2.z);
	printf("---------------------LI----------------------\n");
	*/

	glm::vec3 tmp = (1 - t) * p1 + t * p2;
	if (tmp.z > radius) {
		printf("----------------------\n");
		printf("p1: %f %f %f\n", p1.x, p1.y, p1.z);
		printf("p2: %f %f %f\n", p2.x, p2.y, p2.z);
		printf("----------------------\n");
	}
	//return (p1 + p2) / 2.0f;
	return (1 - t) * p1 + t * p2;
}

//marching cubes without isovalues
void cubeMarch::marchingCubesSimple() {
	std::cout << "Marching Cubes Simple: ... ";

	for (int i = 0; i < voxels.size(); i++) {
		//check what vertice is inside the sphere
		string bin = "00000000";
		float densities[8]; //density of every vertice
		
		//for every vertice
		for (int d = 0; d < 8; d++) {
			densities[d] = getDensity(voxels[i].p[d]); //get the density of the vertice
			if (densities[d] < 0) bin[7 - d] = '1'; //check if the vertice is in the surface
		}


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
		if (edgesInters & 1) vertices[0] = ( getIntersVertice(voxels[i].p[0], voxels[i].p[1], densities[0], densities[1])); //edge 0
		if (edgesInters & 2) vertices[1] = ( getIntersVertice(voxels[i].p[1], voxels[i].p[2], densities[1], densities[2]) ); //edge 1
		if (edgesInters & 4) vertices[2] = ( getIntersVertice(voxels[i].p[2], voxels[i].p[3], densities[2], densities[3]) ); //edge 2
		if (edgesInters & 8) vertices[3] = ( getIntersVertice(voxels[i].p[3], voxels[i].p[0], densities[3], densities[0]) ); //edge 3 
		if (edgesInters & 16) vertices[4] = ( getIntersVertice(voxels[i].p[4], voxels[i].p[5], densities[4], densities[5]) ); //edge 4
		if (edgesInters & 32) vertices[5] = ( getIntersVertice(voxels[i].p[5], voxels[i].p[6], densities[5], densities[6]) ); //edge 5
		if (edgesInters & 64) vertices[6] = ( getIntersVertice(voxels[i].p[6], voxels[i].p[7], densities[6], densities[7]) ); //edge 6
		if (edgesInters & 128) vertices[7] = ( getIntersVertice(voxels[i].p[7], voxels[i].p[4], densities[7], densities[4]) ); //edge 7
		if (edgesInters & 256) vertices[8] = ( getIntersVertice(voxels[i].p[0], voxels[i].p[4], densities[0], densities[4]) ); //edge 8
		if (edgesInters & 512) vertices[9] = ( getIntersVertice(voxels[i].p[1], voxels[i].p[5], densities[1], densities[5]) ); //edge 9
		if (edgesInters & 1024) vertices[10] = ( getIntersVertice(voxels[i].p[2], voxels[i].p[6], densities[2], densities[6]) ); //edge 10
		if (edgesInters & 2048) vertices[11] = ( getIntersVertice(voxels[i].p[3], voxels[i].p[7], densities[3], densities[7]) ); //edge 11


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

	std::vector<glm::vec3> normals;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);
	for (int i = 0; i < meshTriangles.size(); i += 3) {
		glm::vec3 a, b, c, normal;
		a = meshTriangles[i];
		b = meshTriangles[i + 1];
		c = meshTriangles[i + 2];

		normal = -glm::normalize(glm::triangleNormal(a, b, c));

		normals.push_back(normal);
		normals.push_back(normal);
		normals.push_back(normal);
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
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
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

	constexpr float ratio = (float) 1920 / (float) 1080;
	projection = glm::perspective(glm::radians(camera.Zoom), ratio, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
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
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	//std::cout << "[DONE]" << std::endl;
}