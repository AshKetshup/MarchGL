#ifndef CUBEMARCH_H
#define CUBEMARCH_H

#include <shader_m.h>
#include <shader_c.h>

#include <camera.h>

#include <random>

#include "marchingutils.h"

typedef struct {
	glm::vec4 p[8];
} VOXEL;


typedef struct {
	glm::vec4 colorMesh;
	glm::vec4 colorLight;
	bool cameraLightSnap;
	glm::vec3 lightPos;
} SHADER_SETTINGS;

class cubeMarch {
	public:
	Shader basicShader;								//color shader
	Shader shader;									//
	unsigned VAO, meshVAO, gridVAO, gridLinesVAO;
	float radius = 1.0f;							//radius of the sphere
	std::vector<float> vertices;					//vertices of the sphere
	float gridDist = 0.5f;							//distance between each vertice of the VOXELS (dist between every point in the grid)
	int gridPoints;									//total grid points (for drawing)
	std::vector<float> cmgrid;						//vertices of the grid
	std::vector<VOXEL> voxels;									//voxels
	std::vector<glm::vec3> meshTriangles;			//vertices for the triangles (mesh)
	std::vector<glm::vec3> meshTrianglesCS;
	string obj;
	glm::vec4 triangles[10000];						//vertices for the triangles (mesh)
	
	int width, height = 0;

	//torus
	float R1 = 1.0f;
	float R2 = 0.5f;

	//box limit
	glm::vec3 box_lim = glm::vec3(0.0f);

	cubeMarch(std::string obj = "sphere");

	//----sphere (for comparison)----
	void createSphere();
	void drawSphere(Camera camera);

	//----grid----
	void createGrid();
	void drawGrid(Camera camera);

	//-----marching cubes algorithm-----

	//checks if the point is inside or outside the sphere
	float getDensity(glm::vec3 p);

	//returns the middle point between the two vertices
	glm::vec3 getIntersVertice(glm::vec3 p1, glm::vec3 p2, float D1, float D2);

	//marching cubes without isovalues
	void marchingCubesSimple();

	//----mesh----
	void createMesh();
	void drawMesh(Camera camera, glm::vec3 trans, SHADER_SETTINGS& settings);

	void marchingCubesCS();
	void drawGridCS(Camera camera);

	void drawMeshCS(Camera camera);
};

#endif