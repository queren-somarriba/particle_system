#include "shader.hpp"
#include "mat4f.hpp"
#include <fstream>
#include <sstream>


Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::string	vertexCode, fragmentCode;
	std::ifstream vShaderFile, fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch(std::ifstream::failure const& e)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << "\n";
	}
	
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	
	unsigned int vertex   = compileShader(GL_VERTEX_SHADER,   vShaderCode);
	unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

	if (vertex == 0 || fragment == 0)
	{
		if (vertex)
			glDeleteShader(vertex);
		if (fragment)
			glDeleteShader(fragment);
		return;
	}

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	_valid = checkProgramLink(id);
	if (!_valid)
	{
		glDeleteProgram(id);
		id = 0;
	}
}

Shader::~Shader()
{
	if (this->id)
		glDeleteProgram(this->id);
}

bool Shader::isValid() const
{
	return _valid;
}

void Shader::use() const
{
	glUseProgram(this->id);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(this->id, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string &name, int value) const
{ 
	glUniform1i(glGetUniformLocation(this->id, name.c_str()), value); 
}
void Shader::setFloat(const std::string &name, float value) const
{ 
	glUniform1f(glGetUniformLocation(this->id, name.c_str()), value); 
}

void Shader::setVec4(const std::string &name, const vect4f &value) const
{ 
	glUniform4fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string &name, const mat4f &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

unsigned int Shader::compileShader(GLenum type, const char* source)
{
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	int  success;
	char infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		const char* typeName = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
		std::cerr << "ERROR::SHADER::" << typeName
					<< "::COMPILATION_FAILED:\n" << infoLog << "\n";
		glDeleteShader(shader);
		return 0;
	}
	
	return shader;
}

bool Shader::checkProgramLink(unsigned int programId)
{
	int  success;
	char infoLog[512];

	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED:\n"
					<< infoLog << "\n";
		return false;
	}

	return true;
}