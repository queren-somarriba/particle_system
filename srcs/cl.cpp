#include "particle_system.hpp"
#include "cl.hpp"
#include <fstream>
#include <sstream>

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
	// cl_uint num_platforms = 0;
	// cl_int err1 = clGetPlatformIDs(0, NULL, &num_platforms);

	// printf("Nombre de plateformes trouvees : %d (Code retour : %d)\n", num_platforms, err1);

	// std::cout << "platformID: " << clGetPlatformIDs(1, &(data.platform), nullptr) << std::endl;
	// std::cout << "deviceID: " << clGetDeviceIDs(data.platform, CL_DEVICE_TYPE_GPU, 1, &(data.device), nullptr) << std::endl;
	clGetPlatformIDs(1, &(data.platform), nullptr);
	clGetDeviceIDs(data.platform, CL_DEVICE_TYPE_GPU, 1, &(data.device), nullptr);

	GLXContext glx_ctx = glXGetCurrentContext();
	Display* glx_dpy = glXGetCurrentDisplay();
	if (!glx_ctx || !glx_dpy)
		throw std::runtime_error("No GLX active context to create OpenCL context!");

	cl_context_properties properties[] = {
		CL_GL_CONTEXT_KHR,  (cl_context_properties)glx_ctx,
		CL_GLX_DISPLAY_KHR, (cl_context_properties)glx_dpy,
		CL_CONTEXT_PLATFORM,(cl_context_properties)(data.platform),
		0
	};

	cl_int err;
	data.context = clCreateContext(properties, 1, &(data.device), nullptr, nullptr, &err);
	if (err != CL_SUCCESS)
		std::cerr << "Error: clCreateContext. Code: " << err << std::endl;

	data.queue = clCreateCommandQueueWithProperties(data.context, data.device, nullptr, nullptr);
	clEnqueueWriteBuffer(data.queue, data.cl_physics_mem, CL_TRUE, 0, data.particle_number * sizeof(GpuPhysicalParticle),
		nullptr, 0, nullptr, nullptr);
}

void initInteropAndKernel(psData& data)
{
	cl_int err;
	
	glFinish(); 

	data.cl_vbo_mem = clCreateFromGLBuffer(data.context, CL_MEM_READ_WRITE, static_cast<cl_GLuint>(data.vbo->id), &err);
	if (err != CL_SUCCESS) {
		std::cerr << "Error: clCreateFromGLBuffer. Code: " << err << std::endl;
	}

	data.cl_physics_mem = clCreateBuffer(data.context, CL_MEM_READ_WRITE,
		data.particle_number * sizeof(GpuPhysicalParticle), nullptr, &err);

	if (err != CL_SUCCESS)
		std::cerr << "Error: clCreateBuffer. Code: " << err << std::endl;

	std::string source_str = loadKernelSource("./kernels/simulation.cl");
	size_t source_size = source_str.length();
	const char* kernel_source = source_str.c_str();

	data.program = clCreateProgramWithSource(data.context, 1, &kernel_source, &source_size, &err);

	err = clBuildProgram(data.program, 1, &(data.device), "-cl-std=CL2.0", nullptr, nullptr);

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