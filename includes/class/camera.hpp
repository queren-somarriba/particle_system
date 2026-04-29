#pragma once

#include <glad/glad.h>
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

constexpr static float YAW			= -(M_PI / 2.f);
constexpr static float PITCH		= 0.f;
constexpr static float SPEED		= 2.5f;
constexpr static float SENSITIVITY	= 0.001f;
constexpr static float ZOOM			= 45.f;

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

				Camera(vect4f pos = vect4f(0.f, 0.f, 0.f),
						vect4f up = vect4f(0.f, 1.f, 0.f),
						float yaw = YAW,
						float pitch = PITCH);
				~Camera() = default;
				Camera(const Camera&) = delete;
		Camera&	operator=(const Camera&) = delete;
		mat4f	GetViewMatrix();
		void	ProcessKeyboard(Camera_Movement direction, float deltaTime);
		void	ProcessMouseScroll(float yoffset);

			private:
		void	updateCameraVectors();
};
