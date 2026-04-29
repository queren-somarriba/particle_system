#include "camera.hpp"

Camera::Camera(vect4f pos, vect4f up, float yaw, float pitch) :
	pos(pos),
	front(vect4f(0.0f, 0.0f, -1.0f)),
	worldUp(up),
	yaw(yaw),
	pitch(pitch),
	movementSpeed(SPEED),
	mouseSensitivity(SENSITIVITY),
	zoom(ZOOM)
{
	updateCameraVectors();
}

mat4f Camera::GetViewMatrix()
{
	return mat4f::lookAt(pos, pos + front, up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;

	if (direction ==FORWARD)
		this->pos += this->front * velocity;
	if (direction == BACKWARD)
		this->pos -= this->front * velocity;
	if (direction == RIGHT)
		this->pos += this->right * velocity;
	if (direction == LEFT)
		this->pos -= this->right * velocity;
	if (direction == UP)
		this->pos += this->up * velocity;
	if (direction == DOWN)
		this->pos -= this->up * velocity;
}

void Camera::ProcessMouseScroll(float yoffset)
{
	this->zoom -= (float)yoffset;
	if (this->zoom < 1.0f)
		this->zoom = 1.0f;
	if (this->zoom > 45.0f)
		this->zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	vect4f front;
	front.x = std::cos(yaw) * std::cos(pitch);
	front.y = std::sin(pitch);
	front.z = std::sin(yaw) * std::cos(pitch);

	this->front = front.normalize();
	this->right = front.cross(worldUp).normalize();
	this->up = right.cross(front).normalize();
}