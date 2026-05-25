#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "particle_system.hpp"
#include "callback.hpp"
#include "cl.hpp"

GLFWwindow* initWindow()
{
	glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
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
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

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
		state.gpuState.deltaTime = currentFrame - state.lastFrame;
		state.lastFrame = currentFrame;
		state.second += state.gpuState.deltaTime;
		state.gpuState.time += state.gpuState.deltaTime;
		++state.fpsCounter;
		if (state.second >= 1.f)
		{
			float fps = static_cast<float>(state.fpsCounter) / state.second;
			std::stringstream ss;
			ss << "Particle System - " << std::fixed << std::setprecision(0) << fps;
			ss << " fps / " << std::fixed << std::setprecision(1) << 1000.f / fps << " ms";
			glfwSetWindowTitle(window, ss.str().c_str());
			state.fpsCounter = 0;
			state.second = 0;
		}

		mat4f view = state.camera.GetViewMatrix();
		mat4f proj = mat4f::perspective(state.camera.zoom * (float)M_PI / 180.0f, (float)WIDTH/HEIGHT, 0.1f);
		mat4f world = proj * view;
		state.invWorld = world.inverse();
	}

	void setUniformVal(psData& data)
	{
		mat4f view = data.state.camera.GetViewMatrix();
		mat4f proj = mat4f::perspective(data.state.camera.zoom * (float)M_PI / 180.0f, (float)WIDTH/HEIGHT, 0.01f);	
		data.shader->use();
		data.shader->setMat4("proj", proj);
		data.shader->setMat4("view", view);
	}
}

void renderParticles(GLFWwindow* window, psData& data)
{
	processInput(window, data);
	updateAppState(window, data.state);

	clEnqueueWriteBuffer(data.queue, data.cl_state_mem, CL_TRUE, 0, sizeof(GpuSimulationState),
							&data.state.gpuState, 0, nullptr, nullptr);

	clSetKernelArg(data.kernel, 0, sizeof(cl_mem), &data.cl_vbo_mem);
	clSetKernelArg(data.kernel, 1, sizeof(cl_mem), &data.cl_physics_mem);
	clSetKernelArg(data.kernel, 2, sizeof(cl_mem), &data.cl_state_mem);

	glFinish();
	clEnqueueAcquireGLObjects(data.queue, 1, &(data.cl_vbo_mem), 0, nullptr, nullptr);

	size_t global_work_size = data.particle_number;
	clEnqueueNDRangeKernel(data.queue, data.kernel, 1, nullptr, &global_work_size, nullptr, 0, nullptr, nullptr);

	clEnqueueReleaseGLObjects(data.queue, 1, &(data.cl_vbo_mem), 0, nullptr, nullptr);
	clFinish(data.queue);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setUniformVal(data); 	

	data.vao.bind();
	glDrawArrays(GL_POINTS, 0,  data.particle_number);
	data.vao.unbind();

	if (data.state.gpuState.cube)
		data.state.gpuState.cube = 0;
	if (data.state.gpuState.sphere)
		data.state.gpuState.sphere = 0;

	glfwSwapBuffers(window);
	glfwPollEvents();
}