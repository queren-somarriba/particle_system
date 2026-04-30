#include <exception>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "camera.hpp"
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
		p.vel = vect4f(randf() * 2.f - 1.f, randf() * 2.f - 1.f, randf() * 2.f - 1.f);
		p.mass = 1.f;
		vect4f gdir = data.state.gCenter - p.pos;
		p.gDist = gdir.length();
		p.color = vect4f(p.gDist, p.gDist * 0.5f, 0.f);
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
	}
	data.vao.bind();
	data.vbo = std::make_unique<VBO>(gpuVector.data(), gpuVector.size() * sizeof(float));

	data.vao.linkAttrib(*data.vbo, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	data.vao.linkAttrib(*data.vbo, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

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

			glfwSetWindowUserPointer(window, &data);

			while (!glfwWindowShouldClose(window))
				renderParticles(window, data);
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