#include <exception>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "camera.hpp"
// #include <cstdlib>
#include <ctime>
#include "particle_system.hpp"
#include "callback.hpp"
#include "mat4f.hpp"
#include "shader.hpp"
#include "Random.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "utils.hpp"
#include "renderer.hpp"

void setupData(psData& data, const char* argv1)
{
	data.particle_number = std::stoi(argv1);
	for (unsigned int i = 0; i < data.particle_number; ++i)
	{
		Particle p;
		p.pos = vect4f(randf() * 2.f - 1.f, randf() * 2.f - 1.f, randf() * 2.f - 1.f);
		p.color = vect4f(1.f, 0.f, 1.f);
		p.vel = vect4f(randf() * 2.f - 1.f, randf() * 2.f - 1.f, randf() * 2.f - 1.f);
		p.mass = 1.f;
		data.particles.push_back(p);
	}
	std::vector<float>	gpuVector;
	for(const Particle& p : data.particles)
	{
		gpuVector.push_back(p.pos.x);
		gpuVector.push_back(p.pos.y);
		gpuVector.push_back(p.pos.z);
		gpuVector.push_back(p.color.x);
		gpuVector.push_back(p.color.y);
		gpuVector.push_back(p.color.z);
		gpuVector.push_back(p.vel.x);
		gpuVector.push_back(p.vel.y);
		gpuVector.push_back(p.vel.z);
		gpuVector.push_back(p.mass);
	}
	data.vao.bind();
	data.vbo = std::make_unique<VBO>(gpuVector.data(), gpuVector.size() * sizeof(float));

	data.vao.linkAttrib(*data.vbo, 0, 3, GL_FLOAT, 10 * sizeof(float), (void*)0);
	data.vao.linkAttrib(*data.vbo, 1, 3, GL_FLOAT, 10 * sizeof(float), (void*)4);
	data.vao.linkAttrib(*data.vbo, 2, 3, GL_FLOAT, 10 * sizeof(float), (void*)8);
	data.vao.linkAttrib(*data.vbo, 3, 1, GL_FLOAT, 10 * sizeof(float), (void*)10);
	
	data.vao.unbind();
	data.vbo->unbind();

	data.shader = std::make_unique<Shader>("./shaders/Particle.vs", "./shaders/Particle.fs");
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./particle_system <particle_number>" << std::endl;
		return 1;
	}

	try
	{
		std::srand((unsigned int)std::time(nullptr));

		GLFWwindow* window = initWindow();
		if (!window)
			return 1;

		{
			psData data = {};
			setupData(data, argv[1]);

			while (!glfwWindowShouldClose(window))
			{
				updateParticles(data);
				renderParticles(window, data);
			}
		}

		glfwDestroyWindow(window);
		glfwTerminate();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return 0;
}