#ifndef MARCHGL_H
#define MARCHGL_H

#include <windows.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <camera.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <filesys.hpp>

#include <GL/gl.h>

#include <filesystem>
#include <iostream>

//#include "implicit.h"
#include "squareMarch.h"
#include <cubeMarch.h>

#include <torusObj.h>

using namespace std;
using namespace glm;

typedef enum {
	NO_ACTION,
	CAMERA_RESET,
	SHADER_RELOAD,
	CHANGE_COLOR,
	CHANGE_SHADER
} action;

typedef enum {
	CPU,
	GPU
} render_mode;

typedef struct {
	vec2 lastPos;
	bool first;
} mouse_data;

typedef struct {
	unsigned width;
	unsigned height;
	unsigned threads;
	const char* rMode;
} Arguments;


class MarchGLException: public exception {
	public:
	MarchGLException(const string& msg): message(msg) { };
	virtual const char* what() const throw( );

	private:
	const string message;
};

class MarchGL {
	public:
	MarchGL(void);
	MarchGL(Arguments);
	~MarchGL(void);

	void rotateModeView(void);
	bool launchSuccessful(void);

	GLFWwindow* getWindow(void);
	Camera& getCamera(void);
	mouse_data getMouseData(void);
	bool getGUIMode(void);
	string getAppDir(void);
	string getAppName(void);
	string getApp(void);
	string getShaderDir(void);
	unsigned getFPS(void);

	void setMouseData(vec2, bool);
	void setFrameBufferSizeCallback(GLFWframebuffersizefun);
	void setMouseButtonCallback(GLFWmousebuttonfun);
	void setCursorPositionCallback(GLFWcursorposfun);
	void setScrollCallback(GLFWscrollfun);
	void setFPS(unsigned int);

	void switchGUIMode(bool);

	void refresh(void);
	void terminate(void);
	void main(void);

	private:

	static constexpr vec3 SURF_DEFAULT_COLOR = vec3(50.f / 255.f, 140.f / 255.f, 235.f / 255.f);
	static constexpr vec3 BG_DEFAULT_COLOR = vec3(0.53f, 0.81f, 0.98f);
	bool success = false;

	unsigned appFPS;
	string appDir;
	string appName;
	string shaderDir;
	string fontDir;
	string resDir;

	//string slash; //???????????????????????????????????????????????????

	string fontName;
	string logoName;

	render_mode rmode;
	unsigned threadAmount;

	Arguments args;
	SHADER_SETTINGS s;

	GLFWwindow* window;
	unsigned scr_width;
	unsigned scr_height;

	ImGuiIO io;
	ImVec4 meshColor = ImVec4(SURF_DEFAULT_COLOR.x, SURF_DEFAULT_COLOR.y, SURF_DEFAULT_COLOR.z, 1.f);
	ImVec4 lightColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	bool cameraLightSnap = false;
	vec3 lightPos = vec3(0.f);



	// Timing 
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float currentFrame;

	Camera camera;
	mouse_data mouse;
	bool guiMode = false;
	bool w_was_pressed = false;

	vec3 surfColor = SURF_DEFAULT_COLOR;
	vec3 bgColor = BG_DEFAULT_COLOR;

	bool initializeGLFW(unsigned int, unsigned int, const char*);
	int initializeGLAD(void);
	void initializeImGUI(void);
	action processInput(void);

	void newFrameUI(void);
	void renderUI(void);
	void terminateImGUI(void);
};

#endif