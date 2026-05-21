#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "particle_system.hpp"
#include "callback.hpp"
#include "utils.hpp"
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
		mat4f proj = mat4f::perspective(data.state.camera.zoom * (float)M_PI / 180.0f, (float)WIDTH/HEIGHT, 0.1f);	
		data.shader->use();
		data.shader->setMat4("proj", proj);
		data.shader->setMat4("view", view);
	}

	// void emitOne(Particle& p, const Emitter& emitter)
	// {
	// 	float currentTime = static_cast<float>(glfwGetTime()); 
	// 	p.pos  = emitter.pos;
	// 	p.vel  = vect4f(
	// 		emitter.initVelMin.x + randf() * (emitter.initVelMax.x - emitter.initVelMin.x),
	// 		emitter.initVelMin.y + randf() * (emitter.initVelMax.y - emitter.initVelMin.y),
	// 		emitter.initVelMin.z + randf() * (emitter.initVelMax.z - emitter.initVelMin.z)
	// 	);
	// 	p.vel.x = std::cos(currentTime) + p.vel.x;
	// 	p.vel.y = std::cos(currentTime) + p.vel.y;
	// 	if (randf() > 0.5f)
	// 		p.vel = -1.f * p.vel;
	// 	p.maxLife = emitter.minLife + randf() * (emitter.maxLife - emitter.minLife);
	// 	p.life = p.maxLife;
	// 	p.mass = 1.f;
	// 	p.alive = true;
	// 	p.color = vect4f(1.f, 0.8f, 0.2f);
	// }

	// void emitParticles(psData& data)
	// {
	// 	if (data.state.emitter.burstPending)
	// 	{
	// 		unsigned int emitted = 0;
	// 		for (Particle& p : data.particles)
	// 		{
	// 			if (emitted >= data.state.emitter.burstCount) break;
	// 			if (!p.alive)
	// 			{
	// 				emitOne(p, data.state.emitter);
	// 				++emitted;
	// 			}
	// 		}
	// 		data.state.emitter.burstPending = false;
	// 	}

	// 	float interval = 1.f / data.state.emitter.rate;
	// 	data.state.emitter.accumulator += data.state.deltaTime;
		
	// 	while (data.state.emitter.accumulator >= interval && data.state.e_pressed)
	// 	{
	// 		data.state.emitter.accumulator -= interval;
	// 		bool emitted = false;
	// 		for (Particle& p : data.particles)
	// 		{
	// 			if (!p.alive)
	// 			{
	// 				emitOne(p, data.state.emitter);
	// 				emitted = true;
	// 				break;
	// 			}
	// 		}
	// 		if (!emitted)
	// 			break;
	// 	}
	// }

	void updateGravityCenter(psData& data)
	{
		const float speed = 2.f;
		vect4f targetDir = data.state.targetCenter - data.state.gpuState.gCenter;
		float dist = targetDir.length();

		if (dist > 1e-5f) 
		{
			float step = speed * data.state.gpuState.deltaTime;
			step >= dist ?
				data.state.gpuState.gCenter = data.state.targetCenter
			:	data.state.gpuState.gCenter += targetDir.normalize() * step;
		}
	}
}

void updateParticleSystem(psData& data)
{
	updateGravityCenter(data);
	//emitParticles(data);

	// updateParticles(data);

	//setupGpuData(data, data.gpuVector);
}

void renderParticles(GLFWwindow* window, psData& data)
{
	processInput(window, data);
	updateAppState(window, data.state);

	clSetKernelArg(data.kernel, 0, sizeof(cl_mem), &data.cl_vbo_mem);
	clSetKernelArg(data.kernel, 1, sizeof(cl_mem), &data.cl_physics_mem);
	clSetKernelArg(data.kernel, 2, sizeof(GpuSimulationState), &data.state.gpuState);

	glFinish();
	clEnqueueAcquireGLObjects(data.queue, 1, &(data.cl_vbo_mem), 0, nullptr, nullptr);

	size_t global_work_size = data.particle_number;
	clEnqueueNDRangeKernel(data.queue, data.kernel, 1, nullptr, &global_work_size, nullptr, 0, nullptr, nullptr);

	clEnqueueReleaseGLObjects(data.queue, 1, &(data.cl_vbo_mem), 0, nullptr, nullptr);
	clFinish(data.queue);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setUniformVal(data);
	updateParticleSystem(data);

	data.vao.bind();
	glDrawArrays(GL_POINTS, 0,  data.particle_number);
	data.vao.unbind();

	glfwSwapBuffers(window);
	glfwPollEvents();
}