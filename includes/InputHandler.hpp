#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void processInput(GLFWwindow *window, psData& data);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void cursor_enter_callback(GLFWwindow* window, int entered);