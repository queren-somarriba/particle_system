#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "particle_system.hpp"

GLFWwindow* initWindow(AppState& state);

void updateParticles(psData& data);

void renderParticles(GLFWwindow* window, psData& data);