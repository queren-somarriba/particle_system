#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "particle_system.hpp"
#include "callback.hpp"

GLFWwindow* initWindow()
{
	if (!glfwInit())
	{
		std::cerr << "GLFW initialization failed" << std::endl;
		return NULL;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Particle_System", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return (NULL);
	}
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(POINT_SIZE);

 	return window;	
}

namespace
{
	void updateAppState(GLFWwindow* window, AppState& state)
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		state.deltaTime = currentFrame - state.lastFrame;
		state.lastFrame = currentFrame;
		state.second += state.deltaTime;
		++state.fpsCounter;
		if (state.second >= 1.f)
		{
			float fps = static_cast<float>(state.fpsCounter) / state.second;
			std::stringstream ss;
			ss << "scop - " << std::fixed << std::setprecision(0) << fps;
			ss << " fps / " << std::fixed << std::setprecision(1) << 1000.f / fps << " ms";
			glfwSetWindowTitle(window, ss.str().c_str());
			state.fpsCounter = 0;
			state.second = 0;
		}
	}

	void setUniformVal(psData& data)
	{
		mat4f view = data.state.camera.GetViewMatrix();
		mat4f projection = mat4f::perspective(data.state.camera.zoom, (float)WIDTH/HEIGHT, 0.1f,
														data.state.camera.pos.z);	
		data.shader->use();
		data.shader->setMat4("projection", projection);
		data.shader->setMat4("view", view);
	}
}

void updateParticles(psData& data)
{
	const float G = (data.state.gravity ? data.state.G : 0.f);
    const float softening = 0.1f;
	
	for (Particle& p : data.particles)
	{
		vect4f dir = data.state.gCenter - p.pos;
		p.gDist = dir.length();
		float magnitude = G / (p.gDist * p.gDist + softening);
		vect4f acceleration = dir.normalize() * magnitude;
		p.vel.x += acceleration.x * data.state.deltaTime;
		p.vel.y += acceleration.y * data.state.deltaTime;
		p.vel.z += acceleration.z * data.state.deltaTime;
		p.pos.x += p.vel.x * data.state.deltaTime;
		p.pos.y += p.vel.y * data.state.deltaTime;
		p.pos.z += p.vel.z * data.state.deltaTime;
		p.color = vect4f(p.gDist, p.gDist * 0.5f, 0.f);

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
}

void renderParticles(GLFWwindow* window, psData& data)
{

	processInput(window, data);
	updateAppState(window, data.state);

	glClearColor(0.07f, 0.07f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setUniformVal(data);
	updateParticles(data);

	data.vao.bind();
	glDrawArrays(GL_POINTS, 0,  data.particle_number);
	data.vao.unbind();

	glfwSwapBuffers(window);
	glfwPollEvents();
}