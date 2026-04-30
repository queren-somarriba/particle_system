#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "particle_system.hpp"

namespace
{
/* 	void InputMoveCam(GLFWwindow *window, AppState& state)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			state.camera.ProcessKeyboard(FORWARD, state.movementSpeed);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			state.camera.ProcessKeyboard(BACKWARD, state.movementSpeed);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			state.camera.ProcessKeyboard(LEFT, state.movementSpeed);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			state.camera.ProcessKeyboard(RIGHT, state.movementSpeed);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			state.camera.ProcessKeyboard(UP, state.movementSpeed);
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			state.camera.ProcessKeyboard(DOWN, state.movementSpeed);
	} */
}

void processInput(GLFWwindow *window, psData& data)
{
	//InputMoveCam(window, data.state);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !data.state.space_pressed)
	{
		data.state.gravity = !data.state.gravity;
		data.state.space_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		data.state.space_pressed = false;

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

	glfwGetCursorPos(window, &xpos, &ypos);
	if (xpos >= 0.f && xpos <= static_cast<double>(WIDTH))
		data->state.gCenter.x = (static_cast<float>(xpos) - static_cast<float>(WIDTH) * 0.5) / static_cast<float>(WIDTH);

	if (ypos >= 0.f && ypos <= static_cast<double>(HEIGHT))
		data->state.gCenter.y = (static_cast<float>(HEIGHT) * 0.5f - static_cast<float>(ypos))  / static_cast<float>(HEIGHT);
}