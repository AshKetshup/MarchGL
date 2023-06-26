#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <iostream>
#include "shader_m.h"



using namespace std;


class LogoException: public exception {
	public:
	LogoException(const string& msg): message(msg) { }
	virtual const char* what() const throw ( );
	private:
	const string message;
};


typedef struct {
	string path;
	int width, height;
	int nrChannels;
} ImgData;

class cutScene {
	private:
	Shader shader, shaderBG;
	int width, height = 0;

	ImgData data;
	unsigned VAO, VBO, EBO, texture;
	unsigned VAOs, VBOs, EBOs;

	public:
	cutScene(void);
	cutScene(const char* path);
	void createMesh(const char* path);
	void drawMesh(double iTime, float width, float height);

	~cutScene(void);
};

#endif