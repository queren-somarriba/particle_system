#pragma once

#include "vect4f.hpp"
#include "camera.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "shader.hpp"
#include <vector>
#include <memory>

/* CONST*/

const int	WIDTH = 800;
const int	HEIGHT = 800;
const float	G = 500.0f;
const float	POINT_SIZE = 2.0f;
const float	TRAIL_ALPHA = 0.04f;

/* STRUCT */

struct vect2f
{
	float x, y;
};

struct Particle
{
	vect4f	pos;
	vect4f	color;
	vect4f	vel;
	float	mass;
	float	gDist;
};

struct AppState
{
	Camera	camera;
	vect4f	gCenter;
	float	deltaTime;
	float	lastFrame;
	float	movementSpeed;
	float	second;
	float	G;
	size_t	fpsCounter;
	bool	space_pressed;
	bool	gravity;
	
	AppState() : 
		camera(vect4f(0.0f, 0.0f, 3.0f)),
		gCenter(vect4f(0.f, 0.f, 0.f)),
		deltaTime(0.0f), lastFrame(0.0f),
		G(2.f),
		space_pressed(false),
		gravity(true) {}
};

struct psData
{
	std::vector<Particle>	particles;
	AppState				state;
	VAO						vao;
	std::unique_ptr<VBO>	vbo;
	std::unique_ptr<Shader> shader;
	unsigned int			particle_number;
};