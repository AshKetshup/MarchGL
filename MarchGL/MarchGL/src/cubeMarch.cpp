#include "cubeMarch.h"

#include "cparse/shunting-yard.h";
#include "cparse/builtin-features.h";

#include <random>
#include <glm/gtx/normal.hpp>

cubeMarch::cubeMarch(void) {
	cout << "\tInitializing CParse:";
	cparse_startup();
	cout << "\t[OK]\n" << endl;

	renderSettings.cubeSize = 0.1f;
	renderSettings.gridSize = glm::vec3(1.f);
	renderSettings.renderMode = 0;
	renderSettings.threadAmount = 1;
}

cubeMarch::cubeMarch(RENDER_SETTINGS& rS) {
	cout << "\tInitializing CParse:";
	cparse_startup();
	cout << "\t[OK]\n" << endl;

	renderSettings = rS;
}

void cubeMarch::generateSingle(glm::vec3 currPoint) {
	VOXEL voxel[8];
	int bin = 0b00000000;

	//std::cout << "Defining CurrPoint and Density: ... ";
	for (size_t i = 0; i < 8; i++) {
		voxel[i].p = glm::vec3(
			currPoint.x + renderSettings.cubeSize * tbl::vertexOffset[i][0],
			currPoint.y + renderSettings.cubeSize * tbl::vertexOffset[i][1],
			currPoint.z + renderSettings.cubeSize * tbl::vertexOffset[i][2]
		);
		voxel[i].val = getDensity(voxel[i].p);
		if (voxel[i].val < 0)
			bin |= 1 << i;
	}

	//std::cout << "[DONE]" << std::endl;
	if (bin == 0b00000000 || bin == 0b11111111)
		return;

	//std::cout << "bin: " << bin << std::endl;

	int edgeFlag = tbl::edgeTable[bin];

	glm::vec3 edgeVertices[12];


	if (edgeFlag & 1) edgeVertices[0] = getIntersVertice(voxel[0].p, voxel[1].p, voxel[0].val, voxel[1].val); //edge 0
	if (edgeFlag & 2) edgeVertices[1] = getIntersVertice(voxel[1].p, voxel[2].p, voxel[1].val, voxel[2].val); //edge 1
	if (edgeFlag & 4) edgeVertices[2] = getIntersVertice(voxel[2].p, voxel[3].p, voxel[2].val, voxel[3].val); //edge 2
	if (edgeFlag & 8) edgeVertices[3] = getIntersVertice(voxel[3].p, voxel[0].p, voxel[3].val, voxel[0].val); //edge 3 
	if (edgeFlag & 16) edgeVertices[4] = getIntersVertice(voxel[4].p, voxel[5].p, voxel[4].val, voxel[5].val); //edge 4
	if (edgeFlag & 32) edgeVertices[5] = getIntersVertice(voxel[5].p, voxel[6].p, voxel[5].val, voxel[6].val); //edge 5
	if (edgeFlag & 64) edgeVertices[6] = getIntersVertice(voxel[6].p, voxel[7].p, voxel[6].val, voxel[7].val); //edge 6
	if (edgeFlag & 128) edgeVertices[7] = getIntersVertice(voxel[7].p, voxel[4].p, voxel[7].val, voxel[4].val); //edge 7
	if (edgeFlag & 256) edgeVertices[8] = getIntersVertice(voxel[0].p, voxel[4].p, voxel[0].val, voxel[4].val); //edge 8
	if (edgeFlag & 512) edgeVertices[9] = getIntersVertice(voxel[1].p, voxel[5].p, voxel[1].val, voxel[5].val); //edge 9
	if (edgeFlag & 1024) edgeVertices[10] = getIntersVertice(voxel[2].p, voxel[6].p, voxel[2].val, voxel[6].val); //edge 10
	if (edgeFlag & 2048) edgeVertices[11] = getIntersVertice(voxel[3].p, voxel[7].p, voxel[3].val, voxel[7].val); //edge 11

	//std::cout << "Setting the edge vertices: ... ";
	/*for (size_t edge = 0; edge < 12; edge++) {
		if (edgeFlag & ( 1 << edge ))
			edgeVertices[edge] = getIntersVertice(
				voxel[edge % 8].p,
				voxel[( edge + 1 ) % 8].p,
				voxel[edge % 8].val,
				voxel[( edge + 1 ) % 8].val
			);
	}*/
	//std::cout << "[DONE]" << std::endl;

	//std::cout << "Adding vertices and normals to buffers: ... ";
	for (size_t n = 0; tbl::triTable[bin][n] != -1; n += 3) {
		glm::vec3 a, b, c, normal;

		a = edgeVertices[tbl::triTable[bin][n]];
		b = edgeVertices[tbl::triTable[bin][n + 1]];
		c = edgeVertices[tbl::triTable[bin][n + 2]];

		meshTriangles.push_back(a);
		meshTriangles.push_back(b);
		meshTriangles.push_back(c);

		normal = -glm::normalize(glm::triangleNormal(a, b, c));

		normals.push_back(normal);
		normals.push_back(normal);
		normals.push_back(normal);
	}
	//std::cout << "[DONE]" << std::endl;
}

void cubeMarch::generate(void) {
	if (iFunction == "")
		return;

	std::cout << "LOADING CubeMarch SHADER: ... " << std::endl << std::endl;
	shader = Shader("res/shaders/basicShader_vs.glsl", "res/shaders/basicShader_fs.glsl");
	basicShader = Shader("res/shaders/basicColorShader_vs.glsl", "res/shaders/basicColorShader_fs.glsl");
	if (!shader.wasSuccessful()) {
		std::cout << "Shader was not successful" << std::endl;
		std::cout << shader.getReport() << std::endl;
	} else
		std::cout << std::endl << "[DONE]" << std::endl;

	meshTriangles.clear();
	normals.clear();

	std::cout << "Running Marching Cube" << std::endl;
	for (float x = -renderSettings.gridSize.x; x < renderSettings.gridSize.x; x += renderSettings.cubeSize)
		for (float y = -renderSettings.gridSize.y; y < renderSettings.gridSize.y; y += renderSettings.cubeSize)
			for (float z = -renderSettings.gridSize.z; z < renderSettings.gridSize.z; z += renderSettings.cubeSize)
				generateSingle(glm::vec3(x, y, z));
	std::cout << std::endl << "[DONE]" << std::endl;

	createMesh();
}

void cubeMarch::setIFunction(IMPLICIT_FUNCTION& iF) {
	iFunction = iF.function;
}

//----grid----
//void cubeMarch::createGrid() {
//	std::cout << "Create Grid: ... " << std::endl;
//	std::cout << "|";
//
//	VOXEL tmp;
//
//	for (float x = -box_lim[0]; x < box_lim[0]; x += gridDist) {
//		int tX = ( ( x + box_lim[0] ) / ( box_lim[0] * 2 ) ) * 100 / gridDist;
//		if (tX % 10 == 0)
//			std::cout << "#";
//		for (float y = -box_lim[1]; y < box_lim[1]; y += gridDist)
//			for (float z = -box_lim[2]; z < box_lim[2]; z += gridDist) {
//				//for marching cubes 
//
//				tmp.p[0] = glm::vec3(x, y, z);
//				tmp.p[1] = glm::vec3(x + gridDist, y, z);
//				tmp.p[2] = glm::vec3(x + gridDist, y + gridDist, z);
//				tmp.p[3] = glm::vec3(x, y + gridDist, z);
//				tmp.p[4] = glm::vec3(x, y, z + gridDist);
//				tmp.p[5] = glm::vec3(x + gridDist, y, z + gridDist);
//				tmp.p[6] = glm::vec3(x + gridDist, y + gridDist, z + gridDist);
//				tmp.p[7] = glm::vec3(x, y + gridDist, z + gridDist);
//
//				voxels.push_back(tmp);
//			}
//	}
//	std::cout << std::endl;
//
//	gridPoints = cmgrid.size() / 6;
//	unsigned VBO;
//	glGenVertexArrays(1, &gridVAO);
//	glGenBuffers(1, &VBO);
//
//	glBindVertexArray(gridVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, cmgrid.size() * sizeof(float), &cmgrid[0], GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
//	glEnableVertexAttribArray(1);
//
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) ( 3 * sizeof(float) ));
//
//	std::cout << "[DONE]" << std::endl;
//}

//void cubeMarch::drawGrid(Camera camera) {
//	glm::mat4 projection;
//	glm::mat4 view;
//	glm::mat4 model;
//
//	projection = glm::perspective(glm::radians(camera.Zoom), (float) 1920 / (float) 1080, 0.1f, 100.0f);
//	view = camera.GetViewMatrix();
//	model = glm::mat4(1.0f);
//	basicShader.use();
//	basicShader.setMat4("projection", projection);
//	basicShader.setMat4("view", view);
//	basicShader.setMat4("model", model);
//
//	glBindVertexArray(gridVAO);
//	glEnable(GL_PROGRAM_POINT_SIZE);
//	glPointSize(5.0);
//	glDrawArrays(GL_POINTS, 0, gridPoints);
//}



//-----marching cubes algorithm-----


//returns the density of the point p
float cubeMarch::getDensity(glm::vec3 p) {
	// if (obj == "torus") {
	//    //x = (R1 + R2*cos(v)) * cos(u)
	//    //y = (R1 + R2*cos(v)) * sin(u)
	//    //z = R2 * sin(v)
	//    //(sqrt(x^2+y^2) - R)^2 + z^2 = r^2

	//    return ( pow(sqrt(p.x * p.x + p.y * p.y) - R1, 2) + p.z * p.z - R2 * R2 );
	// }

	// //----default----
	// //x^2 + y^2 + z^2 = r^2 (sphere)
	// return ( p.x * p.x + p.y * p.y + p.z * p.z - radius * radius );

	double result = 0.f;
	TokenMap vars;

	vars["x"] = p.x;
	vars["y"] = p.y;
	vars["z"] = p.z;

	try {
		packToken calc = calculator::calculate(iFunction.c_str(), vars);

		result = calc.asDouble();
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	return result;
}

//returns the middle point between the two vertices
glm::vec3 cubeMarch::getIntersVertice(glm::vec3 p1, glm::vec3 p2, float D1, float D2) {
	if (abs(D1) < 0.00001)
		return p1;

	if (abs(D2) < 0.00001)
		return p2;

	if (abs(D1 - D2) < 0.00001)
		return p1;

	float t = -D1 / ( D2 - D1 );

	/*if (tmp.z > radius) {
		std::cout << "----------------------" << std::endl;
		std::cout << "p1: " << p1.x << ", " << p1.y << ", " << p1.z << std::endl;
		std::cout << "p2: " << p2.x << ", " << p2.y << ", " << p2.z << std::endl;
		std::cout << "----------------------" << std::endl;
	}*/
	//return (p1 + p2) / 2.0f;
	return ( 1 - t ) * p1 + t * p2;
}

//marching cubes without isovalues
//void cubeMarch::marchingCubesSimple() {
//	std::cout << "Marching Cubes Simple: ... ";
//
//	for (int i = 0; i < voxels.size(); i++) {
//		//check what vertice is inside the sphere
//		string bin = "00000000";
//		float densities[8]; //density of every vertice
//
//		//for every vertice
//		for (int d = 0; d < 8; d++) {
//			densities[d] = getDensity(voxels[i].p[d]); //get the density of the vertice
//			if (densities[d] < 0) bin[7 - d] = '1'; //check if the vertice is in the surface
//		}
//
//
//		//cube either doesn't have any vertice in the shape, or as all of them
//		if (bin == "00000000" || bin == "11111111")
//			continue;
//
//		//case number (conversion from binary to decimal)
//		int case_n = std::stoi(bin, 0, 2);
//
//		//edges that intersects the isosurface (binary)
//		int edgesInters = tbl::edgeTable[case_n];
//
//		//get all the vertices for the triangles
//		glm::vec3 vertices[12];
//
//		//no edges intersect the isosurface
//		if (edgesInters == 0)
//			continue;
//
//		// (edgesInters & 1) -> verifies if the the right most bit of edgesInters is 1
//		// this allows to check if the edge 0 is in the edges intersected by the implicit function
//		// f.e: edgesInters = 0011 -> true
//		//      edgesInters = 0100 -> false
//		if (edgesInters & 1) vertices[0] = ( getIntersVertice(voxels[i].p[0], voxels[i].p[1], densities[0], densities[1]) ); //edge 0
//		if (edgesInters & 2) vertices[1] = ( getIntersVertice(voxels[i].p[1], voxels[i].p[2], densities[1], densities[2]) ); //edge 1
//		if (edgesInters & 4) vertices[2] = ( getIntersVertice(voxels[i].p[2], voxels[i].p[3], densities[2], densities[3]) ); //edge 2
//		if (edgesInters & 8) vertices[3] = ( getIntersVertice(voxels[i].p[3], voxels[i].p[0], densities[3], densities[0]) ); //edge 3 
//		if (edgesInters & 16) vertices[4] = ( getIntersVertice(voxels[i].p[4], voxels[i].p[5], densities[4], densities[5]) ); //edge 4
//		if (edgesInters & 32) vertices[5] = ( getIntersVertice(voxels[i].p[5], voxels[i].p[6], densities[5], densities[6]) ); //edge 5
//		if (edgesInters & 64) vertices[6] = ( getIntersVertice(voxels[i].p[6], voxels[i].p[7], densities[6], densities[7]) ); //edge 6
//		if (edgesInters & 128) vertices[7] = ( getIntersVertice(voxels[i].p[7], voxels[i].p[4], densities[7], densities[4]) ); //edge 7
//		if (edgesInters & 256) vertices[8] = ( getIntersVertice(voxels[i].p[0], voxels[i].p[4], densities[0], densities[4]) ); //edge 8
//		if (edgesInters & 512) vertices[9] = ( getIntersVertice(voxels[i].p[1], voxels[i].p[5], densities[1], densities[5]) ); //edge 9
//		if (edgesInters & 1024) vertices[10] = ( getIntersVertice(voxels[i].p[2], voxels[i].p[6], densities[2], densities[6]) ); //edge 10
//		if (edgesInters & 2048) vertices[11] = ( getIntersVertice(voxels[i].p[3], voxels[i].p[7], densities[3], densities[7]) ); //edge 11
//
//
//		// Adds the triangles vertices in the correct order to the meshTriangles list
//		for (int n = 0; tbl::triTable[case_n][n] != -1; n += 3) {
//			meshTriangles.push_back(vertices[tbl::triTable[case_n][n]]);
//			meshTriangles.push_back(vertices[tbl::triTable[case_n][n + 1]]);
//			meshTriangles.push_back(vertices[tbl::triTable[case_n][n + 2]]);
//		}
//
//	}
//
//	std::cout << "[DONE]" << std::endl;
//}

//----mesh----
void cubeMarch::createMesh() {
	std::cout << "Create Mesh: ... ";

	if (meshTriangles.empty() || normals.empty()) {
		std::cout << "Buffers are empty. Can't proceed action." << std::endl;
		return;
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