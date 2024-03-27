#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include <marchShader.h>

class ComputeShader: public MarchShader {
	private:
	string shaderPath;

	public:
	ComputeShader(void) { }
	// Constructor generates the compute shader on the fly
	// ------------------------------------------------------------------------
	ComputeShader(const char* computePath, string iFunction = "") {
		// 0. Define the path of the shader
		report = ShaderReport();
		shaderPath = string(computePath);

		// 1. retrieve the compute source code from filePath
		string computeCode;

		try {
			computeCode = extractContent(shaderPath.c_str());
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_COMPUTE, string(e.what()));
			return;
		}

		if (iFunction != "")
			computeCode = formatGLSL(computeCode, iFunction);

		const char* cShaderCode = computeCode.c_str();

		// 2. Compile Shader
		unsigned int compute;

		// compute shader
		compileShader(compute, cShaderCode, SHADER_COMPUTE);
		if (!checkCompileErrors(compute, SHADER_COMPUTE)) return;

		// Shader Program
		ID = glCreateProgram();

		glAttachShader(ID, compute);
		glLinkProgram(ID);
		if (!checkCompileErrors(ID, SHADER_PROGRAM))
			return;

		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(compute);
	}

	void recompileWithFunctions(string iFunction) {
		// 0. Define the path of the shader
		string computeCode = this->shaderPath;

		// 1. retrieve the compute source code from filePath

		try {
			computeCode = extractContent(shaderPath.c_str());
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_COMPUTE, string(e.what()));
			return;
		}

		if (iFunction != "")
			computeCode = formatGLSL(computeCode, iFunction);

		const char* cShaderCode = computeCode.c_str();

		cout << "__COMPUTE SHADER__" << "\n" << cShaderCode << "\n";

		// 2. Compile Shader
		unsigned int compute;

		// compute shader
		compileShader(compute, cShaderCode, SHADER_COMPUTE);
		if (!checkCompileErrors(compute, SHADER_COMPUTE)) return;

		// Count linked shaders and unlink them
		GLsizei count;
		GLuint* shader = (GLuint*) malloc(sizeof(GLuint));

		glGetAttachedShaders(this->ID, 1, &count, shader);
		for (GLsizei i = 0; i < count; i++) {
			glDetachShader(this->ID, shader[i]);
			glDeleteShader(shader[i]);
		}

		free(shader);
		glDeleteProgram(this->ID);

		// Shader Program
		ID = glCreateProgram();

		glAttachShader(ID, compute);
		glLinkProgram(ID);
		if (!checkCompileErrors(ID, SHADER_PROGRAM))
			return;

		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(compute);
	}


	// dispatch the shader
	// ------------------------------------------------------------------------
	void execute(GLuint numGroupsX = 1, GLuint numGroupsY = 1, GLuint numGroupsZ = 1) const {
		glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	string getComputeShaderPath(void) { return shaderPath; }
};

#endif