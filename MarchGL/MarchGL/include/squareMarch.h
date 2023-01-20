#ifndef SQUAREMARCH_H
#define SQUAREMARCH_H

#include <shader_m.h>
#include <camera.h>

#include <vector>

typedef struct {
	glm::vec3 position;
	glm::vec3 color;
} GRIDPOINTS;

class SquareMarch {
	private:
	int x_lim, y_lim; //border limits
	float dist; //distance between points in grid
	int points; //number of points in grid
	std::vector<glm::vec3> meshVertices; //vertices for every point for every line in the mesh
	GRIDPOINTS* gridMatrix; //matrix with all the points
	unsigned borderVAO, gridVAO, mesh;
	Shader shader;

	public:
	SquareMarch(float x_lim, float y_lim, float dist);

	//frame
	void createBorders();
	void drawBorders(Camera camera);

	//grid
	void createGrid();
	void drawGrid(Camera camera);

	//mesh
	void createCircleMesh();
	void drawMesh(Camera camera);

	glm::vec2 getLimits(void);
	float getDist(void);
	int getPoints(void);
	std::vector<glm::vec3> getMeshVertices(void);
	GRIDPOINTS* getGridMatrix(void);
	unsigned getBorderVAO(void);
	unsigned getGridVAO(void);
	unsigned getMesh(void);
	Shader& getShader(void);

	void setLimits(int limX, int limY);
	void setLimits(glm::vec2 lim);
	void setDist(float d);
	void setPoints(int p);
	void setMeshVertices(std::vector<glm::vec3> mesh);
	void setGridMatrix(GRIDPOINTS* grid);
	void setBorderVAO(unsigned vao);
	void setGridVAO(unsigned vao);
	void setMesh(unsigned m);
	void setShader(Shader s);
};

#endif
