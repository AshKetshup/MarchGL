#ifndef CUBEMARCH_H
#define CUBEMARCH_H

#include <shader_m.h>
#include <camera.h>

#include "marchingutils.h"

typedef struct {
	glm::vec3 p;
	float val;
} VOXEL;


typedef struct {
	glm::vec4 colorMesh;
	glm::vec4 colorLight;
	bool cameraLightSnap;
	glm::vec3 lightPos;
} SHADER_SETTINGS;

typedef struct {
	int renderMode;
	int threadAmount;
	float cubeSize;
	glm::vec3 gridSize;
} RENDER_SETTINGS;

typedef struct {
	std::string function;
} IMPLICIT_FUNCTION;

class cubeMarch {
	public:
	Shader basicShader;								// Grid shader
	Shader shader;									// Mesh shader
	unsigned VAO, meshVAO, gridVAO, gridLinesVAO;
	float radius = 1.0f;							// radius of the sphere
	std::vector<float> vertices;					// vertices of the sphere
	RENDER_SETTINGS renderSettings;

	float gridDist = 0.1f;							// distance between each vertice of the VOXELS (dist between every point in the grid)
	int gridPoints;									// total grid points (for drawing)
	std::vector<float> cmgrid;						// vertices of the grid
	std::vector<VOXEL> voxels;						// voxels
	std::vector<glm::vec3> meshTriangles, normals;			// vertices for the triangles (mesh)
	string obj;
	string iFunction;

	int width, height = 0;

	//box limit
	glm::vec3 box_lim;

	cubeMarch(void);
	cubeMarch(RENDER_SETTINGS& rS);
	void setIFunction(IMPLICIT_FUNCTION& iF);

	//----sphere (for comparison)----
	// void createSphere();
	// void drawSphere(Camera camera);

	//----grid----
	// void createGrid();
	// void drawGrid(Camera camera);

	//-----marching cubes algorithm-----
	//checks if the point is inside or outside the sphere
	float getDensity(glm::vec3 p);
	//returns the middle point between the two vertices
	glm::vec3 getIntersVertice(glm::vec3 p1, glm::vec3 p2, float D1, float D2);
	void generateSingle(glm::vec3 currPoint);
	void generate(void);


	//marching cubes without isovalues
	// void marchingCubesSimple();

	//----mesh----
	void createMesh();
	void drawMesh(Camera camera, glm::vec3 trans, SHADER_SETTINGS& settings);
};

#endif