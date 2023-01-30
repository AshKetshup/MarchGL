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

	size = glm::ivec3(
		renderSettings.gridSize.x * 2,
		renderSettings.gridSize.y * 2,
		renderSettings.gridSize.z * 2
	);

	std::cout << "LOADING CubeMarch SHADER: ... " << std::endl << std::endl;
	shader = Shader("res/shaders/basicShader_vs.glsl", "res/shaders/basicShader_fs.glsl");
	basicShader = Shader("res/shaders/basicColorShader_vs.glsl", "res/shaders/basicColorShader_fs.glsl");
	if (!shader.wasSuccessful()) {
		std::cout << "Shader was not successful" << std::endl;
		std::cout << shader.getReport() << std::endl;
		return;
	} else
		std::cout << std::endl << "[DONE]" << std::endl << std::endl;

	std::cout << "LOADING CubeMarch COMPUTE SHADER: ... " << std::endl << std::endl;
	computeShader = ComputeShader("res/shaders/marchingcubes_cs.glsl");
	if (!computeShader.wasSuccessful()) {
		std::cout << "Shader was not successful" << std::endl;
		std::cout << computeShader.getReport() << std::endl;
		return;
	} else
		std::cout << std::endl << "[DONE]" << std::endl;
}

cubeMarch::cubeMarch(RENDER_SETTINGS& rS) {
	cparse_startup();
	size = glm::ivec3(
		renderSettings.gridSize.x * 2,
		renderSettings.gridSize.y * 2,
		renderSettings.gridSize.z * 2
	);

	std::cout << "LOADING CubeMarch SHADER: ... " << std::endl << std::endl;
	shader = Shader("res/shaders/basicShader_vs.glsl", "res/shaders/basicShader_fs.glsl");
	basicShader = Shader("res/shaders/basicColorShader_vs.glsl", "res/shaders/basicColorShader_fs.glsl");
	if (!shader.wasSuccessful()) {
		std::cout << "Shader was not successful" << std::endl;
		std::cout << shader.getReport() << std::endl;
		return;
	}
	else
		std::cout << std::endl << "[DONE]" << std::endl << std::endl;

	std::cout << "LOADING CubeMarch COMPUTE SHADER: ... " << std::endl << std::endl;
	computeShader = ComputeShader("res/shaders/marchingcubes_cs.glsl");
	if (!computeShader.wasSuccessful()) {
		std::cout << "Shader was not successful" << std::endl;
		std::cout << computeShader.getReport() << std::endl;
		return;
	}
	else
		std::cout << std::endl << "[DONE]" << std::endl;




	totalVertices = 0;
	renderSettings = rS;
	cout << "ALL SETTINGS" << endl;
	cout << "Cube Size: " << renderSettings.cubeSize << endl;
	cout << "Grid Size: " << renderSettings.gridSize.x << " " << renderSettings.gridSize.y << " " << renderSettings.gridSize.z << endl;
	cout << "Render Mode: " << renderSettings.renderMode << endl;
	cout << "Thread Amount: " << renderSettings.threadAmount << endl;
	
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
		totalVertices += 3;

		normal = -glm::normalize(glm::triangleNormal(a, b, c));

		normals.push_back(normal);
		normals.push_back(normal);
		normals.push_back(normal);
	}
	//std::cout << "[DONE]" << std::endl;
}


void cubeMarch::generateCPU(void) {
	for (float x = -renderSettings.gridSize.x; x < renderSettings.gridSize.x; x += renderSettings.cubeSize)
		for (float y = -renderSettings.gridSize.y; y < renderSettings.gridSize.y; y += renderSettings.cubeSize)
			for (float z = -renderSettings.gridSize.z; z < renderSettings.gridSize.z; z += renderSettings.cubeSize)
				generateSingle(glm::vec3(x, y, z));
}

/*
void cubeMarch::generateGPU(void) {
	computeShader.recompileWithFunctions(iFunction);

	glm::ivec3 sizeGrid = ( renderSettings.gridSize * glm::vec3(2) ) / renderSettings.cubeSize;

	GLuint meshBuffer, normalBuffer;

	glGenBuffers(1, &meshBuffer);
	glGenBuffers(1, &normalBuffer);

	const int bufferSize = sizeGrid.x * sizeGrid.y * sizeGrid.z * 16;

	glm::vec3* gMeshTriangles = new glm::vec3[bufferSize];
	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gMeshTriangles), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, meshBuffer);


	glm::vec3* gNormals = new glm::vec3[bufferSize];
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gNormals), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, normalBuffer);

	//glBindVertexArray(0);

	for (size_t i = 0; i < bufferSize; i++) {
		cout << gNormals[i].x << ", " << gNormals[i].y << ", " << gNormals[i].z << endl;
		meshTriangles.push_back(gMeshTriangles[i]);
		normals.push_back(gNormals[i]);
	}

	computeShader.use();
	computeShader.setFloat("voxelSize", renderSettings.cubeSize);
	computeShader.setVec3("gridSize", sizeGrid);

	computeShader.execute(sizeGrid.x, sizeGrid.y, sizeGrid.z);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, normalBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize * sizeof(glm::vec3), &gNormals);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize * sizeof(glm::vec3), &gMeshTriangles);
}
*/


//change the parameters in the compute shader
bool cubeMarch::changeComputeShader(glm::ivec3 sizes, string iFunction) {
	// File to be read
	string fileName = "res/shaders/computeShaderOriginal.cs";

	// Open the file in input mode
	fstream file;
	file.open(fileName, ios::in);

	// Check if the file is open
	if (!file) {
		cout << "Error opening fileeee" << endl;
		return 1;
	}

	// Create a string to store the file contents
	string fileContents;

	// Read the file into the string
	while (file) {
		string line;
		getline(file, line);
		fileContents += line + '\n';
	}

	// Close the file
	file.close();

	// Replace the old string with the new string
	size_t pos = 0;
	string oldString = "size1";
	string newString = std::to_string(sizes.x);
	while ((pos = fileContents.find(oldString, pos)) != string::npos) {
		fileContents.replace(pos, oldString.length(), newString);
		pos += newString.size();
	}

	pos = 0;
	oldString = "size2";
	newString = std::to_string(sizes.y);
	while ((pos = fileContents.find(oldString, pos)) != string::npos) {
		fileContents.replace(pos, oldString.length(), newString);
		pos += newString.size();
	}

	pos = 0;
	oldString = "size3";
	newString = std::to_string(sizes.z);
	while ((pos = fileContents.find(oldString, pos)) != string::npos) {
		fileContents.replace(pos, oldString.length(), newString);
		pos += newString.size();
	}

	pos = 0;
	oldString = "iFunction";
	while ((pos = fileContents.find(oldString, pos)) != string::npos) {
		fileContents.replace(pos, oldString.length(), iFunction);
		pos += iFunction.size();
	}

	


	// Open the file for output
	file.open("res/shaders/computeShader.cs", ios::out);

	// Check if the file is open
	if (!file) {
		cout << "Error opening file" << endl;
		return 1;
	}

	// Write the modified file contents to the file
	file << fileContents;

	// Close the file
	file.close();

	cout << "File modified successfully!" << endl;
	return 0;
}


//---------------------------------------------------------GPU------------------------------------

void cubeMarch::generateGPU() {
	glm::ivec3 sizeGrid = (renderSettings.gridSize * glm::vec3(2)) / renderSettings.cubeSize;
	cout << "Going to change the CS" << endl;
	cout << "iFunction: " << iFunction << endl;
	changeComputeShader(sizeGrid, iFunction);
	
	//compile the CS
	ComputeShader computeShader("res/shaders/computeShader.cs");

	//computation sizes
	int maxWorkGroupSize[3], workGroupCounts[3], workGroupInvocations[3];
	//limit for the local size (inside the shader)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroupSize[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxWorkGroupSize[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxWorkGroupSize[2]);

	//limit for the dispatch size
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCounts[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCounts[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCounts[2]);


	printf("MAXIMUM WORK GROUP SIZE: %d %d %d\n", maxWorkGroupSize[0], maxWorkGroupSize[1], maxWorkGroupSize[2]);
	printf("MAXIMUM WORK GROUP COUNT: %d %d %d\n", workGroupCounts[0], workGroupCounts[1], workGroupCounts[2]);


	if (sizeGrid.x > workGroupCounts[0] || sizeGrid.y > workGroupCounts[1] || sizeGrid.z > workGroupCounts[2]) {
		printf("ERROR: WORK GROUP COUNTS ARE TOO BIG\n");
		return;
	}




	//create buffers
	GLuint gridBuffer, edgeTableVBO, triTableVBO; //inputs
	GLuint triangles, normals; //outputs
	glGenBuffers(1, &gridBuffer);
	glGenBuffers(1, &edgeTableVBO);
	glGenBuffers(1, &triTableVBO);
	glGenBuffers(1, &triangles);
	glGenBuffers(1, &normals);


	//bind buffers
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridBuffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4) * gridPoints.size(), &gridPoints[0], GL_STATIC_DRAW);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, gridBuffer); //atrib=0

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, edgeTableVBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(tbl::edgeTable), tbl::edgeTable, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, edgeTableVBO); //atrib=1

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triTableVBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(tbl::triTable), tbl::triTable, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, triTableVBO); //atrib=2


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangles);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TRIANGLES) * sizeGrid.x * sizeGrid.y * sizeGrid.z, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, triangles); //atrib=3

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, normals);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TRIANGLES) * sizeGrid.x * sizeGrid.y * sizeGrid.z, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, normals); //atrib=4


	cout << "Starting compute shader" << endl;
	cout << "Total size" << sizeGrid.x * sizeGrid.y * sizeGrid.z << endl;
	cout << "Sizes: " << sizeGrid.x <<" " << sizeGrid.y << " "<< sizeGrid.z << endl;
	//compute shader
	computeShader.use();
	computeShader.setFloat("dist", renderSettings.cubeSize);
	//computeShader.setFloat("radius", 1.0f);
	//computeShader.setInt("obj", 0);
	computeShader.setFloat("x_size", renderSettings.gridSize.x);
	computeShader.setFloat("y_size", renderSettings.gridSize.y);
	computeShader.setFloat("z_size", renderSettings.gridSize.z);
	glDispatchCompute(sizeGrid.x / 10, sizeGrid.y / 10, sizeGrid.y / 10);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	//VAO and vertices/normals buffer
	glGenVertexArrays(1, &meshVAO);
	glBindVertexArray(meshVAO);
	glBindBuffer(GL_ARRAY_BUFFER, triangles);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind

	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);


	//total vertices
	totalVertices = sizeGrid.x * sizeGrid.y * sizeGrid.z * 12;
}


void cubeMarch::generate(void) {
	if (iFunction == "")
		return;

	meshTriangles.clear();
	normals.clear();

	std::cout << "Running Marching Cube" << std::endl;
	

	if (renderSettings.renderMode == 0) {
		generateCPU();
		createMesh();
	}
	else {
		generateGPU();
	}
	std::cout << std::endl << "[DONE]" << std::endl << std::endl;

	
}

//------------------------------------------------------------------------------------------------


void cubeMarch::setIFunction(IMPLICIT_FUNCTION& iF) {
	iFunction = iF.function;
}

#pragma region Grid
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
#pragma endregion

//-----marching cubes algorithm-----

//returns the density of the point p
float cubeMarch::getDensity(glm::vec3 p) {
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

	return ( 1 - t ) * p1 + t * p2;
}

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
	glDrawArrays(GL_TRIANGLES, 0, totalVertices);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	//std::cout << "[DONE]" << std::endl;
}