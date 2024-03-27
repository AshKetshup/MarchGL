#ifndef MARCHSHADER_H
#define MARCHSHADER_H

#undef __gl_h_
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <format>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>


#define SHADER_VERTEX         0b0000000001
#define SHADER_FRAGMENT       0b0000000010
#define SHADER_TESSELLATION_C 0b0000000100
#define SHADER_TESSELLATION_E 0b0000001000
#define SHADER_GEOMETRY       0b0000010000
#define SHADER_COMPUTE        0b0000100000
#define SHADER_PROGRAM        0b0001000000

#define TYPE_READING          0b0010000000
#define TYPE_COMPILATION      0b0100000000
#define TYPE_LINKING          0b1000000000

using namespace std;

class ShaderReport {
	public:
	ShaderReport(void): error(0), message("") { }

	ShaderReport(const int err, const string msg): error(err), message(msg) { }

	void setReport(const int err, const string msg) { error = err; message = msg; }

	bool success(void) { return error == 0; }

	const string what() const throw ( ) {
		string log("");
		if (error & TYPE_READING)          log += "Could not read ";
		if (error & TYPE_COMPILATION)      log += "Compilation on ";
		if (error & TYPE_LINKING)          log += "Linking on ";
		if (error & SHADER_VERTEX)         log += "vertex shader ";
		if (error & SHADER_FRAGMENT)       log += "fragment shader ";
		if (error & SHADER_TESSELLATION_C) log += "tessellation controller shader ";
		if (error & SHADER_TESSELLATION_E) log += "tessellation evalutator shader ";
		if (error & SHADER_GEOMETRY)       log += "geometry shader ";
		if (error & SHADER_COMPUTE)        log += "compute shader ";
		if (error & SHADER_PROGRAM)        log += "shader program ";
		log += "(message: " + message + ").";
		return log;
	}

	private:
	int error;

	string message;
};

class MarchShader {
	protected:
	ShaderReport report;

	bool checkCompileErrors(GLuint shader, const int type) {
		GLint success;
		GLchar infoLog[1024];

		unsigned int gl_status, gl_type;

		if (!( type & SHADER_PROGRAM )) {
			gl_status = GL_COMPILE_STATUS;
			gl_type = TYPE_COMPILATION;
		} else {
			gl_status = GL_LINK_STATUS;
			gl_type = TYPE_LINKING;
		}

		glGetShaderiv(shader, gl_status, &success);
		if (success)
			return true;

		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		report.setReport(gl_type | type, string(infoLog));
		return false;
	}

	static string formatGLSL(string GLSLCode, string iFunction) {
		regex regexIFunction = regex("// <IFunction>");

		string GLSLFormatted = "return " + iFunction + ";\n";
		string GLSLFCode = regex_replace(GLSLCode, regexIFunction, GLSLFormatted);

		return GLSLFCode;
	}

	static string extractContent(const char* filePath) {
		stringstream stream;
		ifstream fileStream;

		// ensure ifstream objects can throw exceptions:
		fileStream.exceptions(ifstream::failbit | ifstream::badbit);

		// READS CONTENTS OF FILE
		fileStream.open(filePath);
		stream << fileStream.rdbuf();
		fileStream.close();

		return stream.str();
	}

	void compileShader(unsigned int& shaderID, const char* shaderCode, int shaderBin) {
		switch (shaderBin) {
			case SHADER_VERTEX:
				shaderID = glCreateShader(GL_VERTEX_SHADER);
				break;
			case SHADER_FRAGMENT:
				shaderID = glCreateShader(GL_FRAGMENT_SHADER);
				break;
			case SHADER_TESSELLATION_C:
				shaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
				break;
			case SHADER_TESSELLATION_E:
				shaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
				break;
			case SHADER_GEOMETRY:
				shaderID = glCreateShader(GL_GEOMETRY_SHADER);
				break;
			case SHADER_COMPUTE:
				shaderID = glCreateShader(GL_COMPUTE_SHADER);
				break;
		}

		glShaderSource(shaderID, 1, &shaderCode, NULL);
		glCompileShader(shaderID);
	}

	public:
	unsigned int ID;

	bool wasSuccessful(void) { return report.success(); }

	string getReport(void) { return report.what(); }

	ShaderReport getReportHandler(void) { return report; }

	unsigned int getID() { return ID; }

	// activate the shader
	void use() { glUseProgram(ID); }

	// utility uniform functions
	void setBool(const string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
	}

	void setInt(const string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setVec2(const string& name, const glm::vec2& value) const {
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const string& name, float x, float y) const {
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}

	void setVec3(const string& name, const glm::vec3& value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const string& name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}

	void setVec4(const string& name, const glm::vec4& value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec4(const string& name, float x, float y, float z, float w) {
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}

	void setMat2(const string& name, const glm::mat2& mat) const {
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat3(const string& name, const glm::mat3& mat) const {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat4(const string& name, const glm::mat4& mat) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
};

#endif // !MARCHSHADER_H