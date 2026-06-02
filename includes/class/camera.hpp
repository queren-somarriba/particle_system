#pragma once

#include <glad/glad.h>
#include <cmath>
#include "mat4f.hpp"

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

constexpr static float YAW		= -1.57079632f;
constexpr static float PITCH		= 0.f;
constexpr static float SPEED		= 2.5f;
constexpr static float SENSITIVITY	= 0.001f;
constexpr static float ZOOM		= 41.f;

class Camera
{
public:
	vect4f	pos;
	vect4f	front;
	vect4f	up;
	vect4f	right;
	vect4f	worldUp;
	float	yaw;
	float	pitch;
	float	movementSpeed;
	float	mouseSensitivity;
	float	zoom;

	Camera(vect4f position = vect4f(0.0f, 0.0f, 3.0f), 
	vect4f upVector = vect4f(0.0f, 1.0f, 0.0f), 
	float startYaw = YAW, 
	float startPitch = PITCH);

	~Camera() = default;
	Camera(const Camera&) = default;
	Camera& operator=(const Camera&) = delete;

	mat4f	GetViewMatrix();
	void	ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void	ProcessMouseScroll(float yoffset);

private:
	void	updateCameraVectors();
};