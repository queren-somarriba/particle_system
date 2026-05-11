#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "particle_system.hpp"
#include "shape.hpp"

namespace
{
	void InputMoveCam(GLFWwindow *window, AppState& state)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			state.camera.ProcessKeyboard(FORWARD, state.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			state.camera.ProcessKeyboard(BACKWARD, state.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			state.camera.ProcessKeyboard(RIGHT, state.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			state.camera.ProcessKeyboard(LEFT, state.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			state.camera.ProcessKeyboard(UP, state.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			state.camera.ProcessKeyboard(DOWN, state.deltaTime);
	}
}

void processInput(GLFWwindow *window, psData& data)
{
	InputMoveCam(window, data.state);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !data.state.space_pressed)
	{
		data.state.gravity = !data.state.gravity;
		data.state.space_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		data.state.space_pressed = false;

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !data.state.r_pressed)
	{
		data.state.shape = (data.state.shape == CUBE) ? SPHERE : CUBE;
		resetParticles(data);
		data.state.r_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
		data.state.r_pressed = false;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	(void)xoffset;
	psData* data = reinterpret_cast<psData*>(glfwGetWindowUserPointer(window));
	if (data && (data->state.G >= 1.f || (data->state.G >= 0.f && yoffset > 0.f)) &&
			(data->state.G <= 19.f || (data->state.G <= 20.f && yoffset < 0.f)))
		data->state.G += static_cast<float>(yoffset);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	psData* data = reinterpret_cast<psData*>(glfwGetWindowUserPointer(window));

	float x, y;

	glfwGetCursorPos(window, &xpos, &ypos);
	if (xpos >= 0.f && xpos <= static_cast<double>(WIDTH))
		x = ( 2.f * static_cast<float>(xpos) / static_cast<float>(WIDTH)) - 1.f;

	if (ypos >= 0.f && ypos <= static_cast<double>(HEIGHT))
		y = 1.f -(2.f * static_cast<float>(ypos)  / static_cast<float>(HEIGHT));

	x = std::max(-1.0f, std::min(1.0f, x));
	y = std::max(-1.0f, std::min(1.0f, y));
	//std::cout << "Worldmouse: (" << x << ", " << y << ")\n";

	// vect4f mouseVect = vect4f(x, y, 0.95f , 1.f);

	// std::cout << "invWorld: " << data->state.invWorld << std::endl;

	// vect4f worldPos = data->state.invWorld * mouseVect;

	// if (worldPos.w != 0.0f)
	// {
	// 	worldPos.x /= worldPos.w;
	// 	worldPos.y /= worldPos.w;
	// 	worldPos.z /= worldPos.w;
	// }

	// data->state.gCenter = worldPos;
	// std::cout << "gCenter: " << data->state.gCenter << std::endl;
	vect4f mouseVect = vect4f(x, y, 0.f, 1.f);
	vect4f worldPos = data->state.invWorld * mouseVect;

	if (std::abs(worldPos.w) > 1e-6f)
	{
		worldPos.x /= worldPos.w;
		worldPos.y /= worldPos.w;
		worldPos.z /= worldPos.w;
		worldPos.w  = 0.f;
	}
	data->state.gCenter = worldPos;
}