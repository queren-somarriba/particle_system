#include "VAO.hpp"

VAO::VAO() : id(0)
{
	glGenVertexArrays(1, &(this->id));
}

VAO::~VAO()
{
	if (this->id)
		glDeleteVertexArrays(1, &(this->id));
	this->id = 0;
}

void VAO::linkAttrib(VBO& VBO, GLuint layout, GLuint numComponents,
						GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.unbind();
}

void VAO::bind()
{
	glBindVertexArray(this->id);
}

void VAO::unbind()
{
	glBindVertexArray(0);
}