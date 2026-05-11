#pragma once

#include <glad/glad.h>

class VBO
{
	public:
		GLuint	id;
				VBO(GLfloat* vertices, GLsizeiptr size, GLenum data);
				~VBO();
				VBO(const VBO&) = delete;
		VBO&	operator=(const VBO&) = delete;
		void	bind();
		void	unbind();
};

