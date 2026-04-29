#pragma once

#include <glad/glad.h>
#include "VBO.hpp"

class VAO
{
	public:
		GLuint	id;
				VAO();
				~VAO();
				VAO(const VAO&) = delete;
		VAO&	operator=(const VAO&) = delete;
		void	linkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
		void	bind();
		void	unbind();
};