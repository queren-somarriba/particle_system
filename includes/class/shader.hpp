#pragma once

#include <glad/glad.h>
#include <string>
#include "mat4f.hpp"

class Shader
{
	public:
		GLuint				id;

							Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
							~Shader();
							Shader(const Shader&) = delete;
		Shader&				operator=(const Shader&) = delete;
		void				use() const;
		void				setBool(const std::string& name, bool value) const;
		void				setInt(const std::string& name, int value) const;
		void				setFloat(const std::string& name, float value) const;
		void				setVec4(const std::string &name, const vect4f &value) const;
		void				setMat4(const std::string &name, const mat4f &mat) const;
		bool				isValid() const;
	
	private:
		bool				_valid;

		static unsigned int	compileShader(GLenum type, const char* source);
		static bool			checkProgramLink(unsigned int programId);
};