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
const float	POINT_SIZE = 2.0f;

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

struct GpuPhysicalParticle
{
	vect4f	vel;
	float	mass;
	float	life;
	float	maxLife;
	int	alive;
};

struct alignas(16) GpuSimulationState
{
	vect4f	gCenter = vect4f(0.f, 0.f, 0.f);
	float	G = 2.f;
	float	deltaTime = 0.f;
	float	time = 0.f;
	int		immortal = 1;
	int		turbulence = 0;
	int		gravity = 1;
	int		cube = 1;
	int		sphere = 0;
	int		emitte = 0;
	int		speedColor = 0;
};

struct AppState
{
	Camera	camera;
	GpuSimulationState	gpuState;
	mat4f	invWorld;
	vect4f	targetCenter;
	float	lastFrame;
	float	second;
	size_t	fpsCounter;
	Shape	shape = CUBE;
	bool	space_pressed;
	bool	r_pressed = false;
	bool	l_pressed = false;
	bool	c_pressed = false;
	bool	mouse_in_window = false;
	
	AppState() : 
		camera(vect4f(0.0f, 0.0f, 3.0f)),
		targetCenter(vect4f(0.f, 0.f, 0.f)),
		lastFrame(0.0f), space_pressed(false) {}
};

struct psData
{
	AppState				state;
	GpuSimulationState		gpuState;
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
	cl_mem					cl_physics_mem;
};

void setupGpuData(psData& data, std::vector<float> gpuVector);