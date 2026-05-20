#ifndef CL_TARGET_OPENCL_VERSION
    #define CL_TARGET_OPENCL_VERSION 200
#endif

__kernel void update_particles(__global float4* positions, 
								__global float4* velocities,
								const float4 gravityCenter,
                               const float deltaTime, 
                               ) 
{
	int id = get_global_id(0);

	const float softening = 0.5f;
	const float dampling = 0.995f;

	float4 dir = gravityCenter - positions[id];
	gDist = dir.length();
	if (gDist > 1e-5f)
	{
		float magnitude = G / (gDist * gDist + softening * softening);
		float4 acceleration = dir.normalize() * magnitude;
		velocities[id] = acceleration * deltaTime;
	}
	positions[id] += velocities[id] * deltaTime;
	velocities[id] *= dampling;
}