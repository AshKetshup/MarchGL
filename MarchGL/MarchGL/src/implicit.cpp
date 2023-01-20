#include "implicit.h"

ImplicitSurface::ImplicitSurface() {
	setBuffers();
	setShader();
}

void ImplicitSurface::draw(Camera camera) {
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

	glBindVertexArray(VAO);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(10.0);
	glDrawArrays(GL_POINTS, 0, vertices.size() * 1);
}

void ImplicitSurface::setBuffers() {
	GLuint VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	//----------------------------------------sphere---------------------------------------
	//sphere formula = x^2 + y^2 = r^2
	// (=) y = sqrt(r^2-x^2)

	//2y(y^{2}-3x^{2})(1-z^{2})+(x^{2}+y^{2})^{2}-(9z^{2}-1)(1-z^{2})=0
	float radius = 1.0f;
	for (float x = -1; x < 1; x += 0.01) {
		float y = sqrt(radius * radius - x * x);
		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(0.0f);

		vertices.push_back(x);
		vertices.push_back(-y);
		vertices.push_back(0.0f);
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
}

void ImplicitSurface::setShader() {
	basicShader = Shader("shaders/basicShader.vs", "shaders/basicShader.fs");
	if (basicShader.wasSuccessful()) return;
	printf("Shader compile error!!!");
}