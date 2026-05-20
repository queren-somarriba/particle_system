#include "particle_system.hpp"
#include "cl.hpp"

void initVelMem(psData& data)
{
	std::vector<float>initial_velocities;
	initial_velocities.reserve(data.particle_number * 4);

	for (Particle& p : data.particles)
	{
		initial_velocities.push_back(p.vel.x);
		initial_velocities.push_back(p.vel.y);
		initial_velocities.push_back(p.vel.z);
		initial_velocities.push_back(p.vel.w);
	}

	clEnqueueWriteBuffer(data.queue, data.cl_vel_mem, CL_TRUE, 0, data.particle_number * 4 * sizeof(float),
		initial_velocities.data(), 0, nullptr, nullptr);
}

void initOpenCL(psData& data)
{
	clGetPlatformIDs(1, &(data.platform), nullptr);
	cl_context_properties properties[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
		CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)(data.platform),
		0
	};

	clGetDeviceIDs(data.platform, CL_DEVICE_TYPE_GPU, 1, &(data.device), nullptr);
	data.context = clCreateContext(properties, 1, &(data.device), nullptr, nullptr, nullptr);
	data.queue = clCreateCommandQueueWithProperties(data.context, data.device, nullptr, nullptr);
}

void initInteropAndKernel(psData& data)
{
	data.cl_vbo_mem = clCreateFromGLBuffer(data.context, CL_MEM_READ_WRITE, static_cast<cl_GLuint>(data.vbo->id), nullptr);

	cl_int err;
	data.cl_vel_mem = clCreateBuffer(context, CL_MEM_READ_WRITE,
		data.particle_number * 4 * sizeof(float), nullptr, &err);
	
	if (err != CL_SUCCESS)
		std::cerr << "Error: clCreateBuffer" << std::endl;

	const char* kernel_source = "__kernel void simulate...";
	data.program = clCreateProgramWithSource(data.context, 1, &kernel_source, nullptr, nullptr);
	clBuildProgram(data.program, 1, &(data.device), nullptr, nullptr, nullptr);

	data.kernel = clCreateKernel(data.program, "update_particles", nullptr);
}

void cleanupCLobjects(psData& data)
{
	clReleaseMemObject(data.cl_vbo_mem);
	clReleaseMemObject(data.cl_vel_mem);
	clReleaseKernel(data.kernel);
	clReleaseProgram(data.program);
	clReleaseCommandQueue(data.queue);
	clReleaseContext(data.context);
}