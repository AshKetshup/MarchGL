#ifndef SHADER_H
#define SHADER_H

#undef __gl_h_
#include <glad/glad.h>
#include <glm/glm.hpp>

// #include <format>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

#define SHADER_VERTEX       0b00000001
#define SHADER_FRAGMENT     0b00000010
#define SHADER_GEOMETRY     0b00000100
#define SHADER_COMPUTE      0b00001000
#define SHADER_PROGRAM      0b00010000

#define TYPE_READING        0b00100000
#define TYPE_COMPILATION    0b01000000
#define TYPE_LINKING        0b10000000

using namespace std;

class ShaderReport {
	public:
	ShaderReport(void): error(0), message("") { }

	ShaderReport(const int err, const string msg): error(err), message(msg) { }

	void setReport(const int err, const string msg) {
		error = err; message = msg;
	}

	bool success(void) {
		return error == 0;
	}

	const string what() const throw ( ) {
		string log("");
		if (error & TYPE_READING)       log += "Could not read ";
		if (error & TYPE_COMPILATION)   log += "Compilation on ";
		if (error & TYPE_LINKING)       log += "Linking on ";
		if (error & SHADER_VERTEX)      log += "vertex shader ";
		if (error & SHADER_FRAGMENT)    log += "fragment shader ";
		if (error & SHADER_GEOMETRY)    log += "geometry shader ";
		if (error & SHADER_COMPUTE)     log += "compute shader ";
		if (error & SHADER_PROGRAM)     log += "shader program ";
		log += "(message: " + message + ").";
		return log;
	}

	private:
	int error;

	string message;
};


class Shader {
	public:
	unsigned int ID;
	Shader(void) { }
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr) {
		// 0. Define the path of the shader
		report = ShaderReport();
		vertexShader = string(vertexPath);
		fragmentShader = string(fragmentPath);
		geometryShader = string(( geometryPath != nullptr ) ? geometryPath : "");

		// 1. retrieve the vertex/fragment source code from filePath
		string vertexCode;
		string fragmentCode;
		string geometryCode;
		ifstream vShaderFile;
		ifstream fShaderFile;
		ifstream gShaderFile;

		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

		try {
			vShaderFile.open(vertexPath);
			stringstream vShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			vShaderFile.close();
			vertexCode = vShaderStream.str();

			cout << "-- VERTEX SHADER --" << endl << vertexCode << endl;
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_VERTEX, string(e.what()));
			return;
		}

		try {
			fShaderFile.open(fragmentPath);
			stringstream fShaderStream;
			fShaderStream << fShaderFile.rdbuf();
			fShaderFile.close();
			fragmentCode = fShaderStream.str();

			cout << "-- FRAGMENT SHADER --" << endl << fragmentCode << endl;
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_FRAGMENT, string(e.what()));
			return;
		}

		if (geometryPath != nullptr) {
			try {
				gShaderFile.open(geometryPath);
				stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			} catch (ifstream::failure& e) {
				report.setReport(TYPE_READING | SHADER_GEOMETRY, string(e.what()));
				return;
			}
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// 2. compile shaders
		unsigned int vertex, fragment;

		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		if (!checkCompileErrors(vertex, SHADER_VERTEX)) return;

		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		if (!checkCompileErrors(fragment, SHADER_FRAGMENT)) return;

		// if geometry shader is given, compile geometry shader
		unsigned int geometry;
		if (geometryPath != nullptr) {
			const char* gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			if (!checkCompileErrors(geometry, SHADER_GEOMETRY)) return;
		}

		// shader Program
		ID = glCreateProgram();

		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryPath != nullptr)
			glAttachShader(ID, geometry);

		glLinkProgram(ID);
		if (!checkCompileErrors(ID, SHADER_PROGRAM)) return;
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);
	}


	Shader(const char* vertexPath, const char* fragmentPath, vector<string> iFunction, const char* geometryPath = nullptr) {
		// 0. Define the path of the shader
		report = ShaderReport();
		vertexShader = string(vertexPath);
		fragmentShader = string(fragmentPath);
		geometryShader = string(( geometryPath != nullptr ) ? geometryPath : "");

		// 1. retrieve the vertex/fragment source code from filePath
		string vertexCode;
		string fragmentCode;
		string geometryCode;
		ifstream vShaderFile;
		ifstream fShaderFile;
		ifstream gShaderFile;

		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

		try {
			vShaderFile.open(vertexPath);
			stringstream vShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			vShaderFile.close();
			vertexCode = vShaderStream.str();
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_VERTEX, string(e.what()));
			return;
		}

		try {
			fShaderFile.open(fragmentPath);
			stringstream fShaderStream;
			fShaderStream << fShaderFile.rdbuf();
			fShaderFile.close();
			fragmentCode = fShaderStream.str();
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_FRAGMENT, string(e.what()));
			return;
		}

		if (geometryPath != nullptr) {
			try {
				gShaderFile.open(geometryPath);
				stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			} catch (ifstream::failure& e) {
				report.setReport(TYPE_READING | SHADER_GEOMETRY, string(e.what()));
				return;
			}
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		//cout << " __VERTEX__ \n" << vShaderCode << "\n";
		//cout << "__FRAGMENT__\n" << fShaderCode << "\n";

		// 2. compile shaders
		unsigned int vertex, fragment;

		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		if (!checkCompileErrors(vertex, SHADER_VERTEX)) return;

		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		if (!checkCompileErrors(fragment, SHADER_FRAGMENT)) return;

		// if geometry shader is given, compile geometry shader
		unsigned int geometry;
		if (geometryPath != nullptr) {
			const char* gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			if (!checkCompileErrors(geometry, SHADER_GEOMETRY)) return;
		}

		// shader Program
		ID = glCreateProgram();

		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryPath != nullptr)
			glAttachShader(ID, geometry);

		glLinkProgram(ID);
		if (!checkCompileErrors(ID, SHADER_PROGRAM)) return;
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);
	}


	string toString(vector<string> functions) {
		string iFunction = "";
		for (size_t i = 0; i < functions.size(); i++) {
			iFunction.append("(" + functions[i] + ")");

			if (i != ( functions.size() - 1 ))
				iFunction.append(" * ");
		}

		return iFunction;
	}

	void recompileWithFunctions(vector<string> iFunction) {


		// 1. retrieve the vertex/fragment source code from filePath
		string vertexCode = this->vertexShader;
		string fragmentCode = this->fragmentShader;
		string geometryCode = this->geometryShader;
		ifstream vShaderFile;
		ifstream fShaderFile;
		ifstream gShaderFile;

		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

		try {
			vShaderFile.open(vertexShader);
			stringstream vShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			vShaderFile.close();
			vertexCode = vShaderStream.str();
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_VERTEX, string(e.what()));
			return;
		}

		try {
			fShaderFile.open(fragmentShader);
			stringstream fShaderStream;
			fShaderStream << fShaderFile.rdbuf();
			fShaderFile.close();
			fragmentCode = fShaderStream.str();
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_FRAGMENT, string(e.what()));
			return;
		}

		if (geometryShader.compare("")) {
			try {
				gShaderFile.open(geometryShader);
				stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			} catch (ifstream::failure& e) {
				report.setReport(TYPE_READING | SHADER_GEOMETRY, string(e.what()));
				return;
			}
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		cout << " __VERTEX__ \n" << vShaderCode << "\n";
		cout << "__FRAGMENT__\n" << fShaderCode << "\n";

		// 2. recompile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		if (!checkCompileErrors(vertex, SHADER_VERTEX)) return;

		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		if (!checkCompileErrors(fragment, SHADER_FRAGMENT)) return;

		// if geometry shader is given, compile geometry shader
		unsigned int geometry;
		if (geometryShader.compare("")) {
			const char* gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			if (!checkCompileErrors(geometry, SHADER_GEOMETRY)) return;
		}

		// shader Program
		GLsizei count;
		GLuint* shaders = (GLuint*) malloc(3 * sizeof(GLuint));

		glGetAttachedShaders(this->ID, 3, &count, shaders);
		for (GLsizei i = 0; i < count; i++) {
			glDetachShader(this->ID, shaders[i]);
			glDeleteShader(shaders[i]);
		}

		free(shaders);
		glDeleteProgram(this->ID);

		this->ID = glCreateProgram();

		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryShader.compare(""))
			glAttachShader(ID, geometry);

		glLinkProgram(ID);
		if (!checkCompileErrors(ID, SHADER_PROGRAM)) return;
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryShader.compare(""))
			glDeleteShader(geometry);
	}

	// activate the shader
	// ------------------------------------------------------------------------
	void use() {
		glUseProgram(ID);
	}
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
	}
	// ------------------------------------------------------------------------
	void setInt(const string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const string& name, const glm::vec2& value) const {
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const string& name, float x, float y) const {
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const string& name, const glm::vec3& value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const string& name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const string& name, const glm::vec4& value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const string& name, float x, float y, float z, float w) {
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const string& name, const glm::mat2& mat) const {
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const string& name, const glm::mat3& mat) const {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const string& name, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	string getVertexShaderPath(void) {
		return vertexShader;
	}

	string getFragmentShaderPath(void) {
		return fragmentShader;
	}

	string getGeometryShaderPath(void) {
		return geometryShader;
	}

	bool wasSuccessful(void) {
		return report.success();
	}

	string getReport(void) {
		return report.what();
	}

	ShaderReport getReportHandler(void) {
		return report;
	}

	unsigned int getID() {
		return ID;
	}

	private:
	string vertexShader;
	string fragmentShader;
	string geometryShader;
	ShaderReport report;

	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	bool checkCompileErrors(GLuint shader, const int type) {
		GLint success;
		GLchar infoLog[1024];
		if (!( type & SHADER_PROGRAM )) {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				report.setReport(TYPE_COMPILATION | type, string(infoLog));
				return false;
			}
		} else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				report.setReport(TYPE_LINKING | type, string(infoLog));
				return false;
			}
		}
		return true;
	}
};
#endif
