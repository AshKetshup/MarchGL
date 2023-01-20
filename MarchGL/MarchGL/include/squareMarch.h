#include <glad/glad.h>
#include <vector>
#include <shader_m.h>
#include <camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>


typedef struct {
	vec3 position;
	vec3 color;
} GRIDPOINTS;

class squareMarch {
public:
	int x_lim, y_lim; //border limits
	float dist; //distance between points in grid
	int points; //number of points in grid
	std::vector<vec3> meshVertices; //vertices for every point for every line in the mesh
	GRIDPOINTS *gridMatrix; //matrix with all the points
	GLuint borderVAO, gridVAO, mesh;
	Shader shader;
	squareMarch(float x_lim, float y_lim, float dist) {
		this->x_lim = x_lim;
		this->y_lim = y_lim;
		this->dist = dist;

		createBorders();
		createGrid();
		createCircleMesh();
	}

	//frame
	void createBorders() {
		float vertices[] = {
			-x_lim, y_lim, 0.0f, 0.0f, 0.0f, 0.0f, //tl
			x_lim, y_lim, 0.0f, 0.0f, 0.0f, 0.0f, //tr
			x_lim, -y_lim, 0.0f, 0.0f, 0.0f, 0.0f, //br
			-x_lim, -y_lim, 0.0f, 0.0f, 0.0f, 0.0f, //bl
		};

		

		GLuint VBO;
		glGenVertexArrays(1, &borderVAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(borderVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		// position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
		

		
		

		//shadders
		shader = Shader("shaders/basicColorShader.vs", "shaders/basicColorShader.fs");
		if (!shader.wasSuccessful()) {
			cout << "Shader was not successful" << endl;
		}
	}
	void drawBorders(Camera camera) {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;

		projection = glm::perspective(glm::radians(camera.Zoom), (float)1920 / (float)1080, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
		model = glm::mat4(1.0f);
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);
		
		glBindVertexArray(borderVAO);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		
	}
	
	//grid
	void createGrid() {
		std::vector<float> vertices;

		
		for (float x = -x_lim; x < x_lim; x += dist) {
			for (float y = -y_lim; y <= y_lim; y += dist) {
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(0.0f);

				//circle: x^2 + y^2 = r^2
				//r=1

				if (x * x + y * y <= 1) { //black
					vertices.push_back(0.0f);
					vertices.push_back(0.0f);
					vertices.push_back(0.0f);
				}

				else {//white
					vertices.push_back(1.0f);
					vertices.push_back(1.0f);
					vertices.push_back(1.0f);
				}

				
			}
		}


		points = vertices.size();

		//buffers
		GLuint VBO;
		glGenVertexArrays(1, &gridVAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(gridVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points, &vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	


		//!!!!!ASSUMED THAT SHADERS WERE ALREADY COMPILED
	}
	void drawGrid(Camera camera) {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;

		projection = glm::perspective(glm::radians(camera.Zoom), (float)1920 / (float)1080, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
		model = glm::mat4(1.0f);
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		glBindVertexArray(gridVAO);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(5.0);
		glDrawArrays(GL_POINTS, 0, points);
	}


	//mesh
	void createCircleMesh() {


		for (float y = -y_lim; y <= y_lim; y += dist) {
			for (float x = -x_lim; x <= x_lim; x += dist) {
				
				string square = "0000";

				//circle: x^2 + y^2 = r^2
				//r=1
				if (x * x + y * y <= 1) square[3] = '1'; //bl
				if ((x + dist) * (x + dist) + y * y <= 1) square[2] = '1'; //br
				if ((x + dist) * (x + dist) + (y + dist) * (y + dist) <= 1) square[1] = '1'; //tr
				if (x * x + (y + dist) * (y + dist) <= 1) square[0] = '1'; //tl
					
				
				

				int case_n = std::stoi(square, 0, 2);
				vec3 top = vec3(x+(dist/2), y+dist, 0.0f);
				vec3 bottom = vec3(x + (dist / 2), y, 0.0f);
				vec3 left = vec3(x, (y+dist/2), 0.0f);
				vec3 right = vec3(x+dist, y+(dist/2), 0.0f);

				switch (case_n) {
				case 1:
					meshVertices.push_back(bottom);
					meshVertices.push_back(left);
					break;
				case 2:
					meshVertices.push_back(bottom);
					meshVertices.push_back(right);
					break;
				case 3:
					meshVertices.push_back(left);
					meshVertices.push_back(right);
					break;
				case 4:
					meshVertices.push_back(top);
					meshVertices.push_back(right);
					break;
				case 5:
					meshVertices.push_back(left);
					meshVertices.push_back(top);

					meshVertices.push_back(bottom);
					meshVertices.push_back(right);
					break;
				case 6:
					meshVertices.push_back(top);
					meshVertices.push_back(bottom);
					break;
				case 7:
					meshVertices.push_back(left);
					meshVertices.push_back(top);
					break;
				case 8:
					meshVertices.push_back(left);
					meshVertices.push_back(top);
					break;
				case 9:
					meshVertices.push_back(top);
					meshVertices.push_back(bottom);
					break;
				case 10:
					meshVertices.push_back(left);
					meshVertices.push_back(bottom);

					meshVertices.push_back(top);
					meshVertices.push_back(right);
					break;
				case 11:
					meshVertices.push_back(top);
					meshVertices.push_back(right);
					break;
				case 12:
					meshVertices.push_back(left);
					meshVertices.push_back(right);
					break;
				case 13:
					meshVertices.push_back(bottom);
					meshVertices.push_back(right);
					break;
				case 14:
					meshVertices.push_back(left);
					meshVertices.push_back(bottom);
					break;
				}

			}
		}

		//colors
		std::vector<vec3> colors;
		for (int i = 0; i < meshVertices.size(); i++) {
			colors.push_back(vec3(1.0f, 1.0f, 1.0f));
		}

		GLuint VBO, CBO;
		glGenVertexArrays(1, &mesh);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &CBO);

		glBindVertexArray(mesh);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)* meshVertices.size(), &meshVertices[0], GL_STATIC_DRAW);

		//glBindBuffer(GL_ARRAY_BUFFER, CBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)* colors.size(), colors.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		glEnableVertexAttribArray(0);

		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		//glEnableVertexAttribArray(1);

	}
	void drawMesh(Camera camera) {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;

		projection = glm::perspective(glm::radians(camera.Zoom), (float)1920 / (float)1080, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
		model = glm::mat4(1.0f);
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		glBindVertexArray(mesh);
		glDrawArrays(GL_LINES, 0, meshVertices.size());

		//glPointSize(2.0);
		//glDrawArrays(GL_POINTS, 0, meshVertices.size());
	}

	
};