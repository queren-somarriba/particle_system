#pragma once

#include <glad/glad.h>

class EBO
{
	public:
		GLuint	id;
				EBO(GLuint* indices, GLsizeiptr size);
				~EBO();
				EBO(const EBO&) = delete;
		EBO&	operator=(const EBO&) = delete;
		void	bind();
		void	unbind();
};

