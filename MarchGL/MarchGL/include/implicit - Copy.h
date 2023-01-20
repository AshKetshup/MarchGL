#include <glad/glad.h>
#include <vector>
#include <shader_m.h>
#include <camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>


class implicitSurface {
public:
	Shader basicShader;
	GLuint VAO;
	void setBuffers() {
		GLuint VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		std::vector<float> vertices = this->setVertices();

		static float vertices2[] = {
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
		
		glBindVertexArray(VAO);
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		
	}

	std::vector<float> setVertices() {
		std::vector<float> vertices;
		for (int i = 0; i < 3; i++) {
			vertices.push_back(0.5f+i);
			vertices.push_back(0.5f+i);
			vertices.push_back(0.0f+i);
		}
		return vertices;
		
	}

	void setShader() {
		basicShader = Shader("shaders/basicShader.vs", "shaders/basicShader.fs");
		if (basicShader.wasSuccessful()) return;
		printf("Shader compile error!!!");
	}

	void draw(Camera camera) {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;

		projection = glm::perspective(radians(camera.Zoom), (float)1920 / (float)1080, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
		model = glm::mat4(1.0f);
		basicShader.use();
		basicShader.setMat4("projection", projection);
		basicShader.setMat4("view", view);
		basicShader.setMat4("model", model);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	implicitSurface() {
		setBuffers();
		setShader();
	}
	
};