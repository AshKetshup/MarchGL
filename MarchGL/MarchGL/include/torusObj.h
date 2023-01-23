#ifndef TORUSOBJ_H
#define TORUSOBJ_H

#include <math.h>
#include <vector>
#include <shader_m.h>
#include <camera.h>

#define M_PI 3.1415927


class torusObj {
	public:
	float R1 = 2;
	float R2 = 1;
	float dist = 0.01;
	float gridDist = 0.1f;
	std::vector<float> vertices;
	int points, gridPoints;
	GLuint torusVAO, boxVAO, gridVAO;
	Shader shader;
	std::vector<float> cmgrid;


	torusObj() {
		//shadders
		shader = Shader("shaders/basicColorShader_vs.glsl", "shaders/basicColorShader_fs.glsl");
		if (!shader.wasSuccessful()) {
			cout << "Shader was not successful" << endl;
		}
		createPoints();
		createGrid();
		createBoundingBox();
		printf("asdasdasd\n");
	}


	//torus implicit function
	void createPoints() {

		for (float u = 0; u < 2 * M_PI; u += dist) {
			for (float v = 0; v < 2 * M_PI; v += dist) {
				float x = ( R1 + ( R2 * cos(v) ) ) * cos(u);
				float y = ( R1 + ( R2 * cos(v) ) ) * sin(u);
				float z = R2 * sin(v);

				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);

				vertices.push_back(1.0f);
				vertices.push_back(1.0f);
				vertices.push_back(0.0f);
			}
		}



		points = vertices.size() / 6;

		GLuint VBO;
		glGenVertexArrays(1, &torusVAO);
		glGenBuffers(1, &VBO);


		glBindVertexArray(torusVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) ( 3 * sizeof(float) ));



	}
	void drawPoints(Camera camera) {
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

		glBindVertexArray(torusVAO);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(5.0);
		glDrawArrays(GL_POINTS, 0, points);
	}

	//bounding box
	void createBoundingBox() {
		float vertices[] = {
			//up
			( R1 + R2 ), -( R1 + R2 ), R2, 1.0f, 1.0f, 1.0f,//br
			( R1 + R2 ), ( R1 + R2 ), R2, 1.0f, 1.0f, 1.0f,//tr
			-( R1 + R2 ), ( R1 + R2 ), R2, 1.0f, 1.0f, 1.0f,//tl
			-( R1 + R2 ), -( R1 + R2 ), R2, 1.0f, 1.0f, 1.0f,//bl
			//bottom
			-( R1 + R2 ), -( R1 + R2 ), -R2, 1.0f, 1.0f, 1.0f,//bl
			( R1 + R2 ), -( R1 + R2 ), -R2, 1.0f, 1.0f, 1.0f,//br
			( R1 + R2 ), ( R1 + R2 ), -R2, 1.0f, 1.0f, 1.0f,//tr
			-( R1 + R2 ), ( R1 + R2 ), -R2, 1.0f, 1.0f, 1.0f, //tl
			//sides
			-( R1 + R2 ), -( R1 + R2 ), R2, 1.0f, 1.0f, 1.0f,//bl
			-( R1 + R2 ), -( R1 + R2 ), -R2, 1.0f, 1.0f, 1.0f,//bl
			( R1 + R2 ), -( R1 + R2 ), R2, 1.0f, 1.0f, 1.0f,//br
			( R1 + R2 ), -( R1 + R2 ), -R2, 1.0f, 1.0f, 1.0f,//br
			( R1 + R2 ), ( R1 + R2 ), R2, 1.0f, 1.0f, 1.0f,//tr
			( R1 + R2 ), ( R1 + R2 ), -R2, 1.0f, 1.0f, 1.0f,//tr
			-( R1 + R2 ), ( R1 + R2 ), R2, 1.0f, 1.0f, 1.0f,//tl
			-( R1 + R2 ), ( R1 + R2 ), -R2, 1.0f, 1.0f, 1.0f, //tl
		};

		GLuint VBO;
		glGenVertexArrays(1, &boxVAO);
		glGenBuffers(1, &VBO);


		glBindVertexArray(boxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(float), vertices, GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) ( 3 * sizeof(float) ));




	}
	void drawBox(Camera camera) {
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

		glBindVertexArray(boxVAO);
		// Draw the front face of the cube
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDrawArrays(GL_LINE_LOOP, 4, 4);
		glDrawArrays(GL_LINE_LOOP, 8, 2);
		glDrawArrays(GL_LINE_LOOP, 10, 2);
		glDrawArrays(GL_LINE_LOOP, 12, 2);
		glDrawArrays(GL_LINE_LOOP, 14, 2);



	}

	//grid
	void createGrid() {
		for (float x = -( R1 + R2 ); x <= ( R1 + R2 ); x += gridDist) {
			for (float y = -( R1 + R2 ); y <= ( R1 + R2 ); y += gridDist) {
				for (float z = -R2; z <= R2; z += gridDist) {
					cmgrid.push_back(x);
					cmgrid.push_back(y);
					cmgrid.push_back(z);

					cmgrid.push_back(1.0f);
					cmgrid.push_back(1.0f);
					cmgrid.push_back(1.0f);

				}
			}
		}
		gridPoints = cmgrid.size() / 6;

		GLuint VBO;
		glGenVertexArrays(1, &gridVAO);
		glGenBuffers(1, &VBO);


		glBindVertexArray(gridVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, cmgrid.size() * sizeof(float), &cmgrid[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) ( 3 * sizeof(float) ));



	}
	void drawGrid(Camera camera) {
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
		glPointSize(5.0);
		glDrawArrays(GL_POINTS, 0, gridPoints);
	}


	//marching cubes without isovalues
	void marchingCubesSimple() {
		//for every point in grid
		//create a cube (get all 8 points of that cube)
		//check the positiion of all vertices in regard of the torus
		//check the case 
		//check the table

	}


};

#endif