#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "particle_system.hpp"
#include "callback.hpp"
#include "utils.hpp"

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
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	float hash(unsigned int n)
	{
		n = (n << 13) ^ n;
		n = n * (n * n * 15731u + 789221u) + 1376312589u;
		return 1.f - static_cast<float>(n & 0x7fffffffu) / 1073741824.f;
	}

	float valueNoise3D(float x, float y, float z)
	{
		int ix = static_cast<int>(std::floor(x));
		int iy = static_cast<int>(std::floor(y));
		int iz = static_cast<int>(std::floor(z));
		float fx = x - static_cast<float>(ix);
		float fy = y - static_cast<float>(iy);
		float fz = z - static_cast<float>(iz);

		float ux = fx * fx * (3.f - 2.f * fx);
		float uy = fy * fy * (3.f - 2.f * fy);
		float uz = fz * fz * (3.f - 2.f * fz);

		auto h = [&](int dx, int dy, int dz) {
				return hash(static_cast<unsigned int>((ix + dx) * 1619 + (iy + dy) * 31337 + (iz + dz) * 6271));
			};

		float v000 = h(0,0,0); float v100 = h(1,0,0);
		float v010 = h(0,1,0); float v110 = h(1,1,0);
		float v001 = h(0,0,1); float v101 = h(1,0,1);
		float v011 = h(0,1,1); float v111 = h(1,1,1);

		return v000 + ux*(v100-v000)
			 + uy*(v010-v000)
			 + uz*(v001-v000)
			 + ux*uy*(v000-v100-v010+v110)
			 + uy*uz*(v000-v010-v001+v011)
			 + ux*uz*(v000-v100-v001+v101)
			 + ux*uy*uz*(-v000+v100+v010-v110+v001-v101-v011+v111);
	}

	vect4f turbulence(const vect4f& pos, float time, float strength, float freq)
	{
		float ax = valueNoise3D(pos.x * freq + 0.0f, pos.y * freq + 17.3f, time * freq);
		float ay = valueNoise3D(pos.x * freq + 53.1f, pos.y * freq + 0.0f, time * freq + 5.7f);
		float az = valueNoise3D(pos.x * freq + 91.7f, pos.z * freq + 33.2f, time * freq + 11.3f);
		return vect4f(ax * strength, ay * strength, az * strength);
	}

	void updateAppState(GLFWwindow* window, AppState& state)
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		state.deltaTime = currentFrame - state.lastFrame;
		state.lastFrame = currentFrame;
		state.second += state.deltaTime;
		state.time += state.deltaTime;
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

	void emitOne(Particle& p, const Emitter& emitter)
	{
		float currentTime = static_cast<float>(glfwGetTime()); 
		p.pos  = emitter.pos;
		p.vel  = vect4f(
			emitter.initVelMin.x + randf() * (emitter.initVelMax.x - emitter.initVelMin.x),
			emitter.initVelMin.y + randf() * (emitter.initVelMax.y - emitter.initVelMin.y),
			emitter.initVelMin.z + randf() * (emitter.initVelMax.z - emitter.initVelMin.z)
		);
		p.vel.x = std::cos(currentTime) + p.vel.x;
		p.vel.y = std::cos(currentTime) + p.vel.y;
		if (randf() > 0.5f)
			p.vel = -1.f * p.vel;
		p.maxLife = emitter.minLife + randf() * (emitter.maxLife - emitter.minLife);
		p.life = p.maxLife;
		p.mass = 1.f;
		p.alive = true;
		p.color = vect4f(1.f, 0.8f, 0.2f);
	}

	void emitParticles(psData& data)
	{
		if (data.state.emitter.burstPending)
		{
			unsigned int emitted = 0;
			for (Particle& p : data.particles)
			{
				if (emitted >= data.state.emitter.burstCount) break;
				if (!p.alive)
				{
					emitOne(p, data.state.emitter);
					++emitted;
				}
			}
			data.state.emitter.burstPending = false;
		}

		float interval = 1.f / data.state.emitter.rate;
		data.state.emitter.accumulator += data.state.deltaTime;
		
		while (data.state.emitter.accumulator >= interval && data.state.e_pressed)
		{
			data.state.emitter.accumulator -= interval;
			bool emitted = false;
			for (Particle& p : data.particles)
			{
				if (!p.alive)
				{
					emitOne(p, data.state.emitter);
					emitted = true;
					break;
				}
			}
			if (!emitted)
				break;
		}
	}

	void updateParticles(psData& data)
	{
		const float G = (data.state.gravity ? data.state.G : 0.f);
		const float softening = 0.5f;
		const float dampling = 0.995f;
		const float dt = data.state.deltaTime;

		for (Particle& p : data.particles)
		{
			if (!p.alive)
			{
				p.pos = vect4f(99999.0f, 99999.0f, 99999.0f);
				continue;
			}
			if (!data.state.immortal)
				p.life -= dt;
			if (p.life <= 1e-5f)
			{
				p.alive = false;
				p.pos = vect4f(99999.0f, 99999.0f, 99999.0f);	
				continue;
			}
			
			vect4f dir = data.state.gCenter - p.pos;
			p.gDist = dir.length();
			if (p.gDist > 1e-5f)
			{
				float magnitude = G / (p.gDist * p.gDist + softening * softening);
				vect4f acceleration = dir.normalize() * magnitude;
				p.vel += acceleration * dt;
			}
			if (data.state.t_pressed)
			{
				vect4f turb = turbulence(p.pos, data.state.time, data.state.emitter.turbulenceStrength, data.state.emitter.turbulenceFreq);
				p.vel += turb * dt;
			}
			p.pos += p.vel * dt;
			p.vel *= dampling;

			float t = p.life / p.maxLife;
			float center_dist = 1.f - p.gDist / (p.gDist + 1.f);
			center_dist = std::pow(center_dist, 2.f);
			vect4f colorLife(t, 0.f, 1.f - t);
			p.color = vect4f(
				colorLife.x + center_dist,
				colorLife.y + center_dist,
				colorLife.z + center_dist
			);
		}
	}

	void updateGravityCenter(psData& data)
	{
		const float speed = 2.f;
		vect4f targetDir = data.state.targetCenter - data.state.gCenter;
		float dist = targetDir.length();

		if (dist > 1e-5f) 
		{
			float step = speed * data.state.deltaTime;
			step >= dist ?
				data.state.gCenter = data.state.targetCenter
			:	data.state.gCenter += targetDir.normalize() * step;
		}
	}
}

void updateParticleSystem(psData& data)
{
	updateGravityCenter(data);
	emitParticles(data);

	updateParticles(data);

	std::vector<float>	gpuVector;
	setupGpuData(data, gpuVector);
}

void renderGravityCenter(psData& data)
{
	float gx = data.state.gCenter.x;
	float gy = data.state.gCenter.y;
	float gz = data.state.gCenter.z;

	float vertex[] = {
		gx, gy, gz,
		0.f, 0.f, 0.f
	};

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	float prevSize;
	glGetFloatv(GL_POINT_SIZE, &prevSize);
	glPointSize(6.f);

	data.shader->use();
	glDrawArrays(GL_POINTS, 0, 1);

	glPointSize(prevSize);

	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void renderParticles(GLFWwindow* window, psData& data)
{
	processInput(window, data);
	updateAppState(window, data.state);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setUniformVal(data);
	updateParticleSystem(data);

	renderGravityCenter(data);

	data.vao.bind();
	glDrawArrays(GL_POINTS, 0,  data.particle_number);
	data.vao.unbind();

	glfwSwapBuffers(window);
	glfwPollEvents();
}