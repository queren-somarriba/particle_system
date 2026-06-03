#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "particle_system.hpp"

namespace
{
	void InputMoveCam(GLFWwindow *window, AppState& state)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			state.camera.ProcessKeyboard(FORWARD, state.gpuState.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			state.camera.ProcessKeyboard(BACKWARD, state.gpuState.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			state.camera.ProcessKeyboard(LEFT, state.gpuState.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			state.camera.ProcessKeyboard(RIGHT, state.gpuState.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			state.camera.ProcessKeyboard(UP, state.gpuState.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			state.camera.ProcessKeyboard(DOWN, state.gpuState.deltaTime);
	}
}

void processInput(GLFWwindow *window, psData& data)
{
	InputMoveCam(window, data.state);
	/* Escape */
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	/* Space */
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !data.state.space_pressed)
	{
		data.state.gpuState.gravity = data.state.gpuState.gravity ? 0 : 1;
		data.state.space_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		data.state.space_pressed = false;
	/* C */
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !data.state.c_pressed)
	{
		data.state.gpuState.speedColor = !data.state.gpuState.speedColor;
		data.state.c_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
		data.state.c_pressed = false;
	/* E */
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !data.state.gpuState.emitte)
		data.state.gpuState.emitte = true;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
		data.state.gpuState.emitte = false;
	/* G */
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !data.state.g_pressed)
	{
		data.state.staticGravity = !data.state.staticGravity;
		data.state.g_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
		data.state.g_pressed = false;
	/* L */
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !data.state.l_pressed)
	{
		data.state.gpuState.immortal = !data.state.gpuState.immortal;
		data.state.l_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
		data.state.l_pressed = false;
	/* R */
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !data.state.r_pressed)
	{
		data.state.shape = (data.state.shape == CUBE) ? SPHERE : CUBE;
		data.state.gpuState.cube = data.state.shape == CUBE ? 1 : 0;
		data.state.gpuState.sphere = data.state.shape == SPHERE ? 1 : 0;
		data.state.r_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
		data.state.r_pressed = false;
	/* T */
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !data.state.gpuState.turbulence)
		data.state.gpuState.turbulence = true;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
		data.state.gpuState.turbulence = false;
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
	if (data && (data->state.gpuState.G >= 1.f || (data->state.gpuState.G >= 0.f && yoffset > 0.f)) &&
			(data->state.gpuState.G <= 19.f || (data->state.gpuState.G <= 20.f && yoffset < 0.f)))
		data->state.gpuState.G += static_cast<float>(yoffset);
}

void cursor_enter_callback(GLFWwindow* window, int entered)
{
	psData* data = reinterpret_cast<psData*>(glfwGetWindowUserPointer(window));
	if (entered)
		data->state.mouse_in_window = true;
	else
		data->state.mouse_in_window = false;
} 

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	psData* data = reinterpret_cast<psData*>(glfwGetWindowUserPointer(window));

	if (!data->state.mouse_in_window || data->state.staticGravity)
		return;

	int winWidth, winHeight;
	glfwGetWindowSize(window, &winWidth, &winHeight);

	glfwGetCursorPos(window, &xpos, &ypos);

	float x = 0.f, y = 0.f;

	if (xpos >= 0.0 && xpos <= (double)winWidth)
		x = (2.f * (float)xpos / (float)winWidth) - 1.f;
	if (ypos >= 0.0 && ypos <= (double)winHeight)
		y = 1.f - (2.f * (float)ypos / (float)winHeight);

	x = std::max(-1.0f, std::min(1.0f, x));
	y = std::max(-1.0f, std::min(1.0f, y));

	vect4f mouseVect = vect4f(x, y, 0.f, 1.f);
	vect4f worldPos = data->state.invWorld * mouseVect;

	if (std::abs(worldPos.w) > 1e-6f)
	{
		worldPos.x /= worldPos.w;
		worldPos.y /= worldPos.w;
		worldPos.z = data->state.gpuState.gCenter.z;
	}
	data->state.gpuState.gCenter = worldPos;
}