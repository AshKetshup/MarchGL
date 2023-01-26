#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include <shader_m.h>

class ComputeShader {
	private:
	string computeShader;
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

	public:
	unsigned int ID;
	ComputeShader(void) { }
	// Constructor generates the compute shader on the fly
	// ------------------------------------------------------------------------
	ComputeShader(const char* computePath) {
		// 0. Define the path of the shader
		report = ShaderReport();
		computeShader = string(computePath);

		// 1. retrieve the compute source code from filePath
		string computeCode;
		ifstream cShaderFile;
		cShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

		try {
			cShaderFile.open(computeShader);
			stringstream cShaderStream;
			cShaderStream << cShaderFile.rdbuf();
			cShaderFile.close();
			computeCode = cShaderStream.str();
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_COMPUTE, string(e.what()));
			return;
		}

		const char* cShaderCode = computeCode.c_str();

		// 2. Compile Shader
		unsigned int compute;

		// compute shader
		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &cShaderCode, NULL);
		glCompileShader(compute);
		if (!checkCompileErrors(compute, SHADER_COMPUTE)) return;

		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, compute);
		glLinkProgram(ID);
		if (!checkCompileErrors(ID, SHADER_PROGRAM))
			return;

		glDeleteShader(compute);
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

	string getComputeShaderPath(void) {
		return computeShader;
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
};

#endif