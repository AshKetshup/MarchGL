#ifndef CUBEMARCH_H
#define CUBEMARCH_H

#include <shader_m.h>
#include <camera.h>

#include <random>

#include "marchingutils.h"

typedef struct {
	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 p3;
	glm::vec3 p4;
	glm::vec3 p5;
	glm::vec3 p6;
	glm::vec3 p7;
} VOXEL;

class cubeMarch {
	public:
	Shader basicShader;								//simple shader
	Shader shader;									//color shader
	unsigned VAO, meshVAO, gridVAO, gridLinesVAO;
	float radius = 1.0f;							//radius of the sphere
	std::vector<float> vertices;					//vertices of the sphere
	float gridDist = 0.01f;							//distance between each vertice of the VOXELS (dist between every point in the grid)
	int gridPoints;									//total grid points (for drawing)
	std::vector<float> cmgrid;						//vertices of the grid
	std::vector<VOXEL> voxels;						//voxels 
	std::vector<glm::vec3> meshTriangles;			//vertices for the triangles (mesh)
	string obj;

	//torus
	float R1 = 1.0f;
	float R2 = 0.5f;
	//box limit
	float box_lim[3] = { 0.0f, 0.0f, 0.0f };

	cubeMarch(std::string obj = "sphere");

	//----sphere (for comparison)----
	void createSphere();
	void drawSphere(Camera camera);

	//----grid----
	void createGrid();
	void drawGrid(Camera camera);

	//-----marching cubes algorithm-----

	//checks if the point is inside or outside the sphere
	bool checkIsosurface(glm::vec3 p);

	//returns the middle point between the two vertices
	glm::vec3 getIntersVertice(glm::vec3 p1, glm::vec3 p2);

	//marching cubes without isovalues
	void marchingCubesSimple();

	//----mesh----
	void createMesh();
	void drawMesh(Camera camera, glm::vec3 trans);
};

#endif