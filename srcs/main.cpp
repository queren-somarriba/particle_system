#include <exception>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "camera.hpp"
#include <ctime>
#include <cstring>
#include "particle_system.hpp"
#include "InputHandler.hpp"
#include "mat4f.hpp"
#include "shader.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "renderer.hpp"
#include "cl.hpp"
#include <sstream>

namespace
{

bool checkArgv(const char* argv)
	{
		if (!argv || argv[0] == '\0')
			return false;

		char* endPtr = nullptr;
		long value = std::strtol(argv, &endPtr, 10);

		if (endPtr == argv || *endPtr != '\0')
			return false;

		if (value <= 0 || value > 10000000)
			return false;

		return true;
	}

	void displayControls()
	{
		const std::string RESET = "\033[0m";
		const std::string BOLD  = "\033[1m";
		const std::string CYAN  = "\033[36m";
		const std::string GOLD  = "\033[33m";

		std::cout << GOLD << BOLD << "             🎆  PARTICLE SYSTEM  🎆" << RESET << "\n";
		std::cout << "--------------------------------------------------\n\n";

		std::cout << CYAN << BOLD << "[ CAMERA & NAVIGATION ]" << RESET << "\n";
		std::cout << "  • W / A / S / D     : Move camera (Horizontal plane)\n";
		std::cout << "  • Up / Down         : Move camera (Vertical axis)\n";

		std::cout << CYAN << BOLD << "[ SIMULATION CONTROLS ]" << RESET << "\n";
		std::cout << "  • Mouse Movement    : Gravity center follows the cursor (in dynamic mode)\n";
		std::cout << "  • Spacebar          : Toggle Gravity Center (On/Off)\n";
		std::cout << "  • Mouse Scroll      : Adjust Gravity Strength (+/-)\n";
		std::cout << "  • T (Hold)          : Inject Turbulence\n";
		std::cout << "  • E (Hold)          : Emit Continuous Particles\n\n";

		std::cout << CYAN << BOLD << "[ RENDU & MODES ]" << RESET << "\n";
		std::cout << "  • G                 : Toggle dynamic gravity center\n";
		std::cout << "  • C                 : Color Modes (Distance / Lifetime -> Velocity)\n";
		std::cout << "  • L                 : Toggle Particle Lifespan\n";
		std::cout << "  • R                 : Reset Shapes (Cube -> Sphere)\n\n";

		std::cout << "--------------------------------------------------\n";
		std::cout << "  • ESC               : Quit Application\n";
		std::cout << "--------------------------------------------------" << std::endl;
	}

	void setupGLObjects(psData& data)
	{
		data.vao.bind();
		data.vbo = std::make_unique<VBO>(nullptr,
			data.particle_number * 6 * sizeof(float), GL_DYNAMIC_DRAW);
		data.vao.linkAttrib(*data.vbo, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
		data.vao.linkAttrib(*data.vbo, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		data.vao.unbind();
		data.vbo->unbind();

		data.shader = std::make_unique<Shader>("./shaders/Particle.vs", "./shaders/Particle.fs");
	}
}

int main(int argc, char** argv)
{
	if (argc != 2 || !checkArgv(argv[1]))
	{
		std::cerr << "Usage: ./particle_system <particle_number>" << std::endl;
		return 1;
	}

	try
	{
		AppState tmp = {};
		GLFWwindow* window = initWindow(tmp);
		if (!window)
			return 1;
		psData data = {};
		data.particle_number = std::stol(argv[1]);
		data.state.fbWidth = tmp.fbWidth;
		data.state.fbHeight = tmp.fbHeight;
		
		displayControls();
		if (data.particle_number <= 10000000 && data.particle_number > 0)
		{
			setupGLObjects(data);

			glfwSetWindowUserPointer(window, &data);
			glfwSetCursorPos(window, 0.f, 0.f);
			initOpenCL(data);
			initInteropAndKernel(data);

			while (!glfwWindowShouldClose(window))
				renderParticles(window, data);
			cleanupCLobjects(data);
		}
		else
			std::cout << "Try with something between 1 and 10 000 000 particles!" << std::endl;

		glfwDestroyWindow(window);
		glfwTerminate();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << '\n';
	}
	
	return 0;
}