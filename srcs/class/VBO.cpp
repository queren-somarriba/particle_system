#include "VBO.hpp"

VBO::VBO(GLfloat* vertices, GLsizeiptr size, GLenum data) : id(0)
{
	glGenBuffers(1, &(this->id));
	glBindBuffer(GL_ARRAY_BUFFER, this->id);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, data);
}

VBO::~VBO()
{
	if (this->id)
		glDeleteBuffers(1, &(this->id));
	this->id = 0;
}

void VBO::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->id);
}

void VBO::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}