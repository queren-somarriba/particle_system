#include "EBO.hpp"

EBO::EBO(GLuint* indices, GLsizeiptr size) : id(0)
{
	glGenBuffers(1, &(this->id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

EBO::~EBO()
{
	if (this->id)
		glDeleteBuffers(1, &(this->id));
	this->id = 0;
}

void EBO::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
}

void EBO::unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}