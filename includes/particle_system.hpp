#pragma once

#include "vect4f.hpp"
#include "camera.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "shader.hpp"
#include <vector>
#include <memory>
#include <GL/glx.h>

#ifndef CL_TARGET_OPENCL_VERSION
    #define CL_TARGET_OPENCL_VERSION 200
#endif

#ifdef __APPLE__
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
    #include <CL/cl_gl.h>
#endif

/* CONST*/

const int	WIDTH = 800;
const int	HEIGHT = 800;
const float	G = 2.f;
const float	POINT_SIZE = 2.0f;
const float	TRAIL_ALPHA = 0.04f;

/* STRUCT */

struct vect2f
{
	float x, y;
};

enum Shape
{
	CUBE,
	SPHERE
};

struct Particle
{
	vect4f	pos;
	vect4f	color;
	vect4f	vel;
	float	mass;
	float	gDist;
	float	life;
	float	maxLife;
	bool	alive;
};

struct	Emitter
{
	vect4f	pos;
	vect4f	initVelMin;
	vect4f	initVelMax;
	float	rate;
	float	accumulator;
	float	minLife;
	float	maxLife;
	float	turbulenceStrength;
	float	turbulenceFreq;
	unsigned int	burstCount;
	bool	burstPending;
};

struct AppState
{
	Camera	camera;
	Emitter	emitter;
	mat4f	invWorld;
	vect4f	gCenter;
	vect4f	targetCenter;
	float	deltaTime;
	float	lastFrame;
	float	second;
	float	G;
	float	time;
	size_t	fpsCounter;
	Shape	shape = CUBE;
	bool	space_pressed;
	bool	gravity;
	bool	r_pressed = false;
	bool	e_pressed = false;
	bool	l_pressed = false;
	bool	t_pressed = false;
	bool	immortal = true;
	bool	mouse_in_window = false;
	
	AppState() : 
		camera(vect4f(0.0f, 0.0f, 3.0f)),
		emitter(
			{vect4f(0.f, 0.f, 0.f), vect4f(-0.2f, 0.2f, -0.3f),
			vect4f( 0.2f, 0.8f,  0.3f),	500.f, 0.f, 2.f, 6.f, 0.4f, 1.5f, 200, false}
		),
		gCenter(vect4f(0.f, 0.f, 0.f)),
		targetCenter(vect4f(0.f, 0.f, 0.f)),
		deltaTime(0.0f), lastFrame(0.0f),
		G(2.f), time(0.f),
		space_pressed(false),
		gravity(true) {}
};

struct psData
{
	std::vector<Particle>	particles;
	std::vector<float>		gpuVector;
	AppState				state;
	VAO						vao;
	std::unique_ptr<VBO>	vbo;
	std::unique_ptr<Shader> shader;
	unsigned int			particle_number;
	cl_platform_id			platform;
	cl_device_id			device;
	cl_context				context;
	cl_command_queue		queue;
	cl_program				program;
	cl_kernel				kernel;
	cl_mem					cl_vbo_mem;
	cl_mem					cl_vel_mem;
};

void setupGpuData(psData& data, std::vector<float> gpuVector);