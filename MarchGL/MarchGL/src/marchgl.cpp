#include "marchgl.h"

#include "mutils.h"

namespace callback {
	MarchGL* instance = nullptr;

	void bindInstance(MarchGL* i) {
		instance = i;
	}

	MarchGL* getInstance(void) {
		return instance;
	}

	void framebufferSize(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	}

	void mouseBtn(GLFWwindow* window, int button, int action, int mods) {
		bool leftBtnDown = false;
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (GLFW_PRESS == action)
				leftBtnDown = true;
			else if (GLFW_RELEASE == action)
				leftBtnDown = false;
		}

		if (!leftBtnDown)
			return;

		double x, y;
		glfwGetCursorPos(getInstance()->getWindow(), &x, &y);
	}

	void mouse(GLFWwindow* window, double xpos, double ypos) {
		vec2 pos(xpos, ypos);

		if (getInstance()->getMouseData().first) {
			getInstance()->setMouseData(pos, false);
		}

		vec2 offsets(
			xpos - getInstance()->getMouseData().lastPos.x,
			getInstance()->getMouseData().lastPos.y - ypos
		);

		getInstance()->setMouseData(pos, false);
		getInstance()->getCamera().ProcessMouseMovement(offsets.x, offsets.y);

	}

	void mouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
		getInstance()->getCamera().ProcessMouseScroll(yoffset);
	}
}

const char* MarchGLException::what(void) const throw ( ) {
	return message.c_str();
}

bool MarchGL::initializeGLFW(unsigned int width, unsigned int height, const char* title) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window)
		return false;

	glfwMakeContextCurrent(window);

	setFrameBufferSizeCallback(callback::framebufferSize);
	setMouseButtonCallback(callback::mouseBtn);
	setCursorPositionCallback(callback::mouse);
	setScrollCallback(callback::mouseScroll);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

int MarchGL::initializeGLAD(void) {
	return gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
}

action MarchGL::processInput(void) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	camera.MovementSpeed = ( glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS )
		? 6.f
		: 3.f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		return action::CAMERA_RESET;

	return action::NO_ACTION;
}


Camera& MarchGL::getCamera(void) {
	return camera;
}

mouse_data MarchGL::getMouseData(void) {
	return mouse;
}

void MarchGL::setMouseData(vec2 pos, bool first) {
	mouse = { pos, first };
}

bool MarchGL::getGUIMode(void) {
	return guiMode;
}

void MarchGL::switchGUIMode(bool mode) {
	guiMode = mode;
}

void MarchGL::setFrameBufferSizeCallback(GLFWframebuffersizefun callback) {
	glfwSetFramebufferSizeCallback(window, callback);
}

void MarchGL::setMouseButtonCallback(GLFWmousebuttonfun callback) {
	glfwSetMouseButtonCallback(window, callback);
}

void MarchGL::setCursorPositionCallback(GLFWcursorposfun callback) {
	glfwSetCursorPosCallback(window, callback);
}

void MarchGL::setScrollCallback(GLFWscrollfun callback) {
	glfwSetScrollCallback(window, callback);
}

bool MarchGL::launchSuccessful(void) {
	return success;
}

string MarchGL::getAppDir(void) {
	return appDir;
}

string MarchGL::getAppName(void) {
	return appName;
}

string MarchGL::getApp(void) {
	return appDir + slash + appName;
}

string MarchGL::getShaderDir(void) {
	return shaderDir;
}

GLFWwindow* MarchGL::getWindow(void) {
	return window;
}

unsigned MarchGL::getFPS(void) {
	return appFPS;
}

void MarchGL::setFPS(unsigned fps) {
	appFPS = fps;
}

void MarchGL::refresh(void) {
	callback::bindInstance(this);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	switch (processInput()) {
		case action::CAMERA_RESET:
			camera.Position = vec3(0.f, 1.f, 0.f);
			cout << "Camera Reset." << endl;
			break;
		case action::CHANGE_COLOR:

		case action::CHANGE_SHADER:
		case action::SHADER_RELOAD:

		case action::NO_ACTION:
		default:
			break;
	}


}

//----------------------------------------------------main--------------------------------------------



void MarchGL::main(void) {
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;

	unsigned int counter = 0;
	ImplicitSurface is();
	SquareMarch sm(2, 2, 0.001);

	std::vector<vec3> vec = sm.getMeshVertices();

	while (!glfwWindowShouldClose(getWindow())) {
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;

		if (timeDiff >= 1.0 / 30.0) {
			setFPS(( 1.0 / timeDiff ) * counter);

			prevTime = crntTime;
			counter = 0;
		}

		refresh();

		//is.draw(camera);
		sm.drawBorders(camera);
		//sm.drawGrid(camera);
		sm.drawMesh(camera);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void MarchGL::terminate(void) {
	glfwTerminate();
}

MarchGL::MarchGL(void) { }

MarchGL::MarchGL(Arguments args) {
	try {
		cout << "Launching MarchGL" << endl;

		cout << "\tSetting Global Variables: " << endl;
		if (strcmp(args.rMode, "CPU") == 0) {
			rmode = CPU;
			cout << "\t| Render Mode: CPU" << endl;
		} else if (strcmp(args.rMode, "GPU") == 0) {
			rmode = GPU;
			cout << "\t| Render Mode: GPU" << endl;
		}

		threadAmount = args.threads;
		cout << "\t| Threads: " << threadAmount << endl;

		scr_height = args.height;
		scr_width = args.width;

		cout << "\t| Window Resolution: " << scr_width << "x" << scr_height << endl;
		cout << "\t[OK]\n" << endl;


		cout << "\tSetting Relevant Directories: " << endl;
		filesystem::path appPath(filesys::getAppPath());

		appDir = appPath.parent_path().string();
		cout << "\t| App Dir: " << appDir << endl;

		appName = appPath.filename().string();
		cout << "\t| App Name: " << appName << endl;

		shaderDir = appDir + slash + "shaders";
		cout << "\t| Shaders: " << shaderDir << endl;

		resDir = appDir + slash + "res";
		cout << "\t| Resources: " << resDir << endl;
		cout << "\t[OK]\n" << endl;


		cout << "\tInitializing GLFW & GLAD:" << endl;
		setMouseData(vec2(scr_width / 2.f, scr_width / 2.f), true);

		if (!initializeGLFW(scr_width, scr_height, "MarchGL - IsoSurfaces with Marching Cubes"))
			throw MarchGLException("Failed to create the Window");
		cout << "\t| GLFW: Success" << endl;
		if (!initializeGLAD())
			throw MarchGLException("Failed to initialize GLAD");
		cout << "\t| GLAD: Success" << endl;
		cout << "\t[OK]\n" << endl;


		cout << "\tLoading Starting Shaders: " << endl;

		cout << "\t[OK]\n" << endl;

		cout << "[OK]" << endl;
		cout << "Done.\n" << endl;

	} catch (const MarchGLException& e) {
		cerr << "[Error]: " << e.what() << endl;
		cout << "Abort Launch!" << endl;

		success = false;
	}

	success = true;
}

MarchGL::~MarchGL(void) {
	terminate();
}
