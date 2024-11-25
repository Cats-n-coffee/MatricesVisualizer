#include "ShaderProgram.h"
#include "FileParser.h"

ShaderProgram::ShaderProgram(const std::string& vertexFilePath, const std::string& fragmentFilePath)
	: m_vertexFilePath(vertexFilePath), m_fragmentFilePath(fragmentFilePath)
{
	// Create program
	m_shaderProgram = glCreateProgram();

	m_vertexShader = AddShader(m_vertexFilePath, GL_VERTEX_SHADER);
	m_fragmentShader = AddShader(m_fragmentFilePath, GL_FRAGMENT_SHADER);

	glLinkProgram(m_shaderProgram);
	glUseProgram(m_shaderProgram);

	// TODO: move the line below
	// ensures the "(location = 0)" gets bound to its "position" name
	glBindAttribLocation(m_shaderProgram, 0, "aPosition");
	glBindAttribLocation(m_shaderProgram, 1, "aColor");
}

ShaderProgram::~ShaderProgram()
{
}

unsigned int ShaderProgram::AddShader(const std::string& filePath, GLenum type)
{
	// Parse shader file to string
	std::string shaderFileContent = ReadResourceFileToStr(filePath);
	const char* shaderCodePtr = shaderFileContent.c_str();

	// Create, assign and compile
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderCodePtr, NULL);
	glCompileShader(shader);

	glAttachShader(m_shaderProgram, shader);

	return shader;
}
