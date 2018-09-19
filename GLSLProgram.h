#pragma once

#include <string>
#include <glew.h>

class GLSLProgram
{
public:
	GLSLProgram();
	~GLSLProgram();

	void compileShaders(const std::string &FilePath, const std::string &fragmentShaderFilePath);

	void linkShaders();
	
	void addAttribute(const std::string& attributeName);

	GLint getUniformLocation(const std::string& uniformName);
	
	void use();
	void unuse();
private:
	int _numAttributes;

	GLuint _programID;
	GLuint _vertexShaderID;
	GLuint _fragmentShaderID;

	void compileShader(const std::string& filePath, GLuint id);
};

