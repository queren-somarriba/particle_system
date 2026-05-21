#pragma once

#include "particle_system.hpp"

#define CL_GL_CONTEXT_KHR 0x2008
#define CL_GLX_DISPLAY_KHR 0x200A

void initOpenCL(psData& data);

void initInteropAndKernel(psData& data);

void cleanupCLobjects(psData& data);

void initPhysicsMem(psData& data);