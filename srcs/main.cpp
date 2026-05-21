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
#include "shape.hpp"
#include "cl.hpp"

// void setupGpuData(psData& data, std::vector<float> gpuVector)
// {
// 	data.gpuVector.clear();
// 	gpuVector.reserve(data.particle_number * 6);
	
// 	for(const Particle& p : data.particles)
// 	{
// 		gpuVector.push_back(p.pos.x);
// 		gpuVector.push_back(p.pos.y);
// 		gpuVector.push_back(p.pos.z);
// 		gpuVector.push_back(p.color.x);
// 		gpuVector.push_back(p.color.y);
// 		gpuVector.push_back(p.color.z);
// 	}

// 	data.vbo->bind();
// 	glBufferSubData(GL_ARRAY_BUFFER, 0, gpuVector.size() * sizeof(float), gpuVector.data());
// 	data.vbo->unbind();
// }

void setupData(psData& data, const char* argv1)
{
	data.particle_number = std::stoi(argv1);
	data.particles.resize(data.particle_number);

	initCube(data.particles, data.particle_number, data.state.gpuState.gCenter);

	data.gpuVector.reserve(data.particle_number * 6);
	for (const Particle& p : data.particles)
	{
		data.gpuVector.push_back(p.pos.x);
		data.gpuVector.push_back(p.pos.y);
		data.gpuVector.push_back(p.pos.z);
		data.gpuVector.push_back(p.color.x);
		data.gpuVector.push_back(p.color.y);
		data.gpuVector.push_back(p.color.z);
	}

	data.vao.bind();
	data.vbo = std::make_unique<VBO>(data.gpuVector.data(),
		data.gpuVector.size() * sizeof(float), GL_DYNAMIC_DRAW);
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
			glfwSetCursorPos(window, 0.f, 0.f);
			initOpenCL(data);
			initInteropAndKernel(data);
			initPhysicsMem(data);

			while (!glfwWindowShouldClose(window))
				renderParticles(window, data);
			cleanupCLobjects(data);
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