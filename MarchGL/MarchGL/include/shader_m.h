#ifndef SHADER_H
#define SHADER_H

#include <marchShader.h>

class Shader: public MarchShader {
	public:
	Shader(void) { }
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	Shader(const char* vertexPath, const char* fragmentPath, const char* tessControlPath = nullptr, const char* tessEvalPath = nullptr) {
		// 0. Define the path of the shader
		report = ShaderReport();
		vertexShader = string(vertexPath);
		fragmentShader = string(fragmentPath);
		tessControlShader = string(( tessControlPath != nullptr ) ? tessControlPath : "");
		tessEvalShader = string(( tessEvalPath != nullptr ) ? tessEvalPath : "");

		tessAvailable = ( tessControlPath && tessEvalPath );

		// 1. retrieve the shader source code from filePath
		string vertexCode, fragmentCode, tessControlCode, tessEvalCode;

		try {
			vertexCode = extractContent(vertexPath);

			cout << "-- VERTEX SHADER --" << endl << vertexCode << endl;
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_VERTEX, string(e.what()));
			return;
		}

		try {
			fragmentCode = extractContent(fragmentPath);

			cout << "-- FRAGMENT SHADER --" << endl << fragmentCode << endl;
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_FRAGMENT, string(e.what()));
			return;
		}

		if (tessAvailable) {
			try {
				tessControlCode = extractContent(tessControlPath);
			} catch (ifstream::failure& e) {
				report.setReport(TYPE_READING | SHADER_TESSELLATION_C, string(e.what()));
				return;
			}

			try {
				tessEvalCode = extractContent(tessEvalPath);
			} catch (ifstream::failure& e) {
				report.setReport(TYPE_READING | SHADER_TESSELLATION_E, string(e.what()));
				return;
			}
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// 2. compile shaders
		unsigned int vertex, fragment, tessellationControl, tessellationEval;

		// vertex shader
		compileShader(vertex, vShaderCode, SHADER_VERTEX);
		if (!checkCompileErrors(vertex, SHADER_VERTEX))
			return;

		// fragment Shader
		compileShader(fragment, fShaderCode, SHADER_FRAGMENT);
		if (!checkCompileErrors(fragment, SHADER_FRAGMENT))
			return;

		// if tessellation shaders are given, compile tessellation shaders
		if (tessAvailable) {
			compileShader(tessellationControl, tessControlCode.c_str(), SHADER_TESSELLATION_C);
			if (!checkCompileErrors(tessellationControl, SHADER_TESSELLATION_C))
				return;

			compileShader(tessellationEval, tessEvalCode.c_str(), SHADER_TESSELLATION_C);
			if (!checkCompileErrors(tessellationEval, SHADER_TESSELLATION_E))
				return;
		}

		// shader Program
		ID = glCreateProgram();

		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (tessAvailable) {
			glAttachShader(ID, tessellationControl);
			glAttachShader(ID, tessellationEval);
		}

		glLinkProgram(ID);
		if (!checkCompileErrors(ID, SHADER_PROGRAM))
			return;


		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (tessAvailable) {
			glDeleteShader(tessellationControl);
			glDeleteShader(tessellationEval);
		}
	}

	void recompileWithFunctions(vector<string> iFunction) {
		// 1. retrieve the vertex/fragment source code from filePath
		string vertexCode = vertexShader;
		string fragmentCode = fragmentShader;
		string tessControlCode = tessControlShader;
		string tessEvalCode = tessEvalShader;

		try {
			vertexCode = extractContent(vertexShader.c_str());
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_VERTEX, string(e.what()));
			return;
		}

		try {
			fragmentCode = extractContent(fragmentShader.c_str());
		} catch (ifstream::failure& e) {
			report.setReport(TYPE_READING | SHADER_FRAGMENT, string(e.what()));
			return;
		}

		if (tessControlShader.compare("")) {
			try {
				tessControlCode = extractContent(tessControlShader.c_str());
			} catch (ifstream::failure& e) {
				report.setReport(TYPE_READING | SHADER_TESSELLATION_C, string(e.what()));
				return;
			}
		}

		if (tessEvalShader.compare("")) {
			try {
				tessEvalCode = extractContent(tessEvalShader.c_str());
			} catch (ifstream::failure& e) {
				report.setReport(TYPE_READING | SHADER_TESSELLATION_C, string(e.what()));
				return;
			}
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		cout << " __VERTEX__ \n" << vShaderCode << "\n";
		cout << "__FRAGMENT__\n" << fShaderCode << "\n";

		// 2. recompile shaders
		unsigned int vertex, fragment, tessellationControl, tessellationEval;

		// vertex shader
		compileShader(vertex, vShaderCode, SHADER_VERTEX);
		if (!checkCompileErrors(vertex, SHADER_VERTEX))
			return;

		// fragment Shader
		compileShader(fragment, fShaderCode, SHADER_FRAGMENT);
		if (!checkCompileErrors(fragment, SHADER_FRAGMENT))
			return;

		// if tessellation shader is given, compile tessellation shader
		if (tessControlShader.compare("")) {
			compileShader(tessellationControl, tessControlCode.c_str(), SHADER_TESSELLATION_C);
			if (!checkCompileErrors(tessellationControl, SHADER_TESSELLATION_C))
				return;
		}

		if (tessEvalShader.compare("")) {
			compileShader(tessellationEval, tessEvalCode.c_str(), SHADER_TESSELLATION_C);
			if (!checkCompileErrors(tessellationEval, SHADER_TESSELLATION_C))
				return;
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
		if (tessControlShader.compare(""))
			glAttachShader(ID, tessellationControl);

		glLinkProgram(ID);
		if (!checkCompileErrors(ID, SHADER_PROGRAM)) return;
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (tessControlShader.compare(""))
			glDeleteShader(tessellationControl);
		if (tessEvalShader.compare(""))
			glDeleteShader(tessellationEval);
	}

	// activate the shader
	void use() {
		glUseProgram(ID);
	}

	string getVertexShaderPath(void) { return vertexShader; }

	string getFragmentShaderPath(void) { return fragmentShader; }

	string gettessControlShaderPath(void) { return tessControlShader; }

	bool wasSuccessful(void) { return report.success(); }

	string getReport(void) { return report.what(); }

	ShaderReport getReportHandler(void) { return report; }

	unsigned int getID() { return ID; }

	private:
	string vertexShader;
	string fragmentShader;
	string tessControlShader;
	string tessEvalShader;
	ShaderReport report;

	bool tessAvailable;
};

#endif
