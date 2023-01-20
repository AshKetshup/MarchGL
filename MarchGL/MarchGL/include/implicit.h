#ifndef IMPLICIT_H
#define IMPLICIT_H

#include <vector>
#include <shader_m.h>
#include <camera.h>

class ImplicitSurface {
	private:
	Shader basicShader;
	unsigned VAO;
	float radius = 1.0f;
	std::vector<float> vertices;

	void setBuffers();
	void setShader();

	public:
	void draw(Camera camera);
	ImplicitSurface();
};

#endif