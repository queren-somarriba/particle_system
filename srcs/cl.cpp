#include "particle_system.hpp"
#include "cl.hpp"
#include <fstream>
#include <sstream>
#ifdef __APPLE__
	#include <OpenGL/OpenGL.h>
#endif

namespace
{
	std::string loadKernelSource(const std::string& filename)
	{
		std::ifstream file(filename);
		if (!file.is_open())
			throw std::runtime_error("Failed to open kernel file : " + filename);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
}

void initOpenCL(psData& data)
{
	clGetPlatformIDs(1, &data.platform, nullptr);

	cl_uint num_devices = 0;
	cl_int err;
	clGetDeviceIDs(data.platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &num_devices);
	std::vector<cl_device_id> devices(num_devices);
	clGetDeviceIDs(data.platform, CL_DEVICE_TYPE_GPU, num_devices, devices.data(), nullptr);

	data.device = devices[0];
	for (cl_uint i = 0; i < num_devices; i++)
	{
		char vendor[128] = {};
		clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(vendor), vendor, nullptr);
		std::string v(vendor);
		if (v.find("AMD") != std::string::npos || v.find("ATI") != std::string::npos)// to do - check 42
		{
			data.device = devices[i];
			break;
		}
	}

	size_t ext_size;
	clGetDeviceInfo(data.device, CL_DEVICE_EXTENSIONS, 0, nullptr, &ext_size);
	std::string exts(ext_size, '\0');
	clGetDeviceInfo(data.device, CL_DEVICE_EXTENSIONS, ext_size, (void*)exts.data(), nullptr);
	
	#ifdef __APPLE__
	if (exts.find("cl_APPLE_gl_sharing") == std::string::npos)
		throw std::runtime_error("cl_APPLE_gl_sharing not supported.");
	#else
	if (exts.find("cl_khr_gl_sharing") == std::string::npos)
		throw std::runtime_error("cl_khr_gl_sharing not supported.");
	#endif
	cl_context_properties properties[7] = {};
	int i = 0;

	#if defined(__APPLE__)
	CGLContextObj cgl_ctx = CGLGetCurrentContext();
	CGLShareGroupObj cgl_grp = CGLGetShareGroup(cgl_ctx);
	if (!cgl_ctx)
		throw std::runtime_error("No CGL context found");
	properties[i++] = CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE;
	properties[i++] = (cl_context_properties)cgl_grp;
	#else
	GLXContext glx_ctx = glXGetCurrentContext();
	Display* glx_dpy = glXGetCurrentDisplay();
	if (glx_ctx && glx_dpy)
	{
		properties[i++] = CL_GL_CONTEXT_KHR;
		properties[i++] = (cl_context_properties)glx_ctx;
		properties[i++] = CL_GLX_DISPLAY_KHR;
		properties[i++] = (cl_context_properties)glx_dpy;
	}
	#endif
	properties[i++] = CL_CONTEXT_PLATFORM;
	properties[i++] = (cl_context_properties)data.platform;
	properties[i] = 0;
	data.context = clCreateContext(properties, 1, &data.device, nullptr, nullptr, &err);
	if (err != CL_SUCCESS)
		throw std::runtime_error("clCreateContext failed: " + std::to_string(err));
	#ifdef __APPLE__
		data.queue = clCreateCommandQueue(data.context, data.device, 0, nullptr);
	#else
		data.queue = clCreateCommandQueueWithProperties(data.context, data.device, nullptr, nullptr);
	#endif
}

void initInteropAndKernel(psData& data)
{
	cl_int err;
	
	glFinish(); 

	data.cl_vbo_mem = clCreateFromGLBuffer(data.context, CL_MEM_READ_WRITE, static_cast<cl_GLuint>(data.vbo->id), &err);
	if (err != CL_SUCCESS)
		std::cerr << "Error: clCreateFromGLBuffer. Code: " << err << std::endl;

	data.cl_physics_mem = clCreateBuffer(data.context, CL_MEM_READ_WRITE,
		data.particle_number * sizeof(GpuPhysicalParticle), nullptr, &err);
	if (err != CL_SUCCESS)
		std::cerr << "Error: clCreateBuffer. Code: " << err << std::endl;

	data.cl_state_mem = clCreateBuffer(data.context, CL_MEM_READ_WRITE,
		sizeof(GpuSimulationState), nullptr, &err);
	if (err != CL_SUCCESS)
		std::cerr << "Error: clCreateBuffer. Code: " << err << std::endl;

	float zero = 0.f;
	clEnqueueFillBuffer(data.queue, data.cl_physics_mem, &zero, sizeof(float),
		0, data.particle_number * sizeof(GpuPhysicalParticle), 0, nullptr, nullptr);
	clEnqueueAcquireGLObjects(data.queue, 1, &data.cl_vbo_mem, 0, nullptr, nullptr);
	clEnqueueFillBuffer(data.queue, data.cl_vbo_mem, &zero, sizeof(float),
		0, data.particle_number * 6 * sizeof(float), 0, nullptr, nullptr);
	clEnqueueReleaseGLObjects(data.queue, 1, &data.cl_vbo_mem, 0, nullptr, nullptr);
	clFinish(data.queue);

	data.state.gpuState.cube = 1;

	std::string source_str = loadKernelSource("./kernels/simulation.cl");
	size_t source_size = source_str.length();
	const char* kernel_source = source_str.c_str();

	data.program = clCreateProgramWithSource(data.context, 1, &kernel_source, &source_size, &err);

	#ifdef __APPLE__
		err = clBuildProgram(data.program, 1, &(data.device), "-cl-std=CL1.2", nullptr, nullptr);
	#else
		err = clBuildProgram(data.program, 1, &(data.device), "-cl-std=CL2.0", nullptr, nullptr);
	#endif
	if (err != CL_SUCCESS)
	{
		size_t log_size;
		clGetProgramBuildInfo(data.program, data.device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
		std::vector<char> build_log(log_size);
		clGetProgramBuildInfo(data.program, data.device, CL_PROGRAM_BUILD_LOG, log_size, build_log.data(), nullptr);
		
		std::cerr << "ERROR::KERNEL::COMPILATION" << std::endl;
		std::cerr << build_log.data() << std::endl;
		throw std::runtime_error("Fail to compile OpenCL code");
	}

	data.kernel = clCreateKernel(data.program, "update_particles", &err);
}

void cleanupCLobjects(psData& data)
{
	clReleaseMemObject(data.cl_vbo_mem);
	clReleaseMemObject(data.cl_physics_mem);
	clReleaseKernel(data.kernel);
	clReleaseProgram(data.program);
	clReleaseCommandQueue(data.queue);
	clReleaseContext(data.context);
}