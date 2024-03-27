#include "cutScene.h"

#include <glm/gtc/matrix_transform.hpp>

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


const char* LogoException::what(void) const throw( ) {
	return message.c_str();
}

cutScene::cutScene(void) { }

cutScene::cutScene(const char* path) {
	cout << "LOADING CubeMarch SHADER: ... " << endl << endl;
	shader = Shader("res/shaders/cutscene_vs.glsl", "res/shaders/cutscene_fs.glsl");
	if (!shader.wasSuccessful()) {
		cout << "Shader was not successful" << endl;
		cout << shader.getReport() << endl;
		return;
	} else
		cout << endl << "[DONE]" << endl << endl;

	shaderBG = Shader("res/shaders/cutsceneBG_vs.glsl", "res/shaders/cutsceneBG_fs.glsl");
	if (!shaderBG.wasSuccessful()) {
		cout << "Shader was not successful" << endl;
		cout << shaderBG.getReport() << endl;
		return;
	} else
		cout << endl << "[DONE]" << endl << endl;

	try {
		createMesh(path);
	} catch (const LogoException& e) {
		cout << e.what() << endl;
		return;
	}
}

void cutScene::createMesh(const char* path) {
	float vertices[] = {
		// positions         // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f    // top left 
	};

	float verticesScreen[] = {
		// positions       
		 1.0f,  1.0f, 0.0f,  // top right
		 1.0f, -1.0f, 0.0f,  // bottom right
		-1.0f, -1.0f, 0.0f,  // bottom left
		-1.0f,  1.0f, 0.0f,  // top left 
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &( this->VAOs ));
	glGenBuffers(1, &( this->VBOs ));
	glGenBuffers(1, &( this->EBOs ));

	glBindVertexArray(VAOs);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesScreen), verticesScreen, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position screen attr
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) ( 0 ));
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &( this->VAO ));
	glGenBuffers(1, &( this->VBO ));
	glGenBuffers(1, &( this->EBO ));

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) ( 3 * sizeof(float) ));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) ( 6 * sizeof(float) ));
	glEnableVertexAttribArray(2);


	glGenTextures(1, &( this->texture ));
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data = stbi_load(path, &( this->data.width ), &( this->data.height ), &( this->data.nrChannels ), 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->data.width, this->data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // TODO: FIND BUG
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		stbi_image_free(data);
		throw LogoException("Failed to load logo");
	}

	stbi_image_free(data);

}

void cutScene::drawMesh(double iTime, float width, float height) {
	glEnable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float size = static_cast<float>( ( width < height ) ? width : height );


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	shader.use();

	shader.setFloat("iTime", iTime);
	shader.setInt("ourTexture", 0);
	shader.setMat4("projection", glm::ortho(-1.0f, 1.0f, static_cast<float>( height ) / static_cast<float>( width ), -static_cast<float>( height ) / static_cast<float>( width )));
	shader.setVec2("wSize", { width, height });

	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUseProgram(0);

	shaderBG.use();

	shaderBG.setFloat("iTime", iTime);
	shaderBG.setMat4("projection", glm::ortho(-1.0f, 1.0f, static_cast<float>( height ) / static_cast<float>( width ), -static_cast<float>( height ) / static_cast<float>( width )));
	shaderBG.setVec2("wSize", { width, height });

	glBindVertexArray(this->VAOs);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUseProgram(0);


	glDisable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);

}

cutScene::~cutScene(void) { }