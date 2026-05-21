#ifndef CL_TARGET_OPENCL_VERSION
    #define CL_TARGET_OPENCL_VERSION 200
#endif


/* Structures */

typedef struct
{
	float4	gCenter;
	float	G;
	float	deltaTime;
	float	time;
	int		immortal;
	int		turbulence;
	int		gravity;
	int		cube;
	int		sphere;
} __attribute__ ((aligned (16))) GpuSimulationState;

typedef struct __attribute__ ((packed))
{
	float4	vel;
	float	mass;
	float	life;
	float	maxLife;
	int	alive;
} GpuPhysicalParticle;


/* Functions */
inline float extract_random(__private unsigned int* seed)
{
	*seed += 0xe2211c97;
	unsigned int x = *seed;
	x ^= x >> 16;
	x *= 0x7feb352d;
	x ^= x >> 15;
	x *= 0x846ca68b;
	x ^= x >> 16;
	// Renvoie un float entre 0.0 et 1.0
	return (float)x / 4294967295.f; 
}

inline float hash(unsigned int n)
{
	n = (n << 13) ^ n;
	n = n * (n * n * 15731u + 789221u) + 1376312589u;
	return 1.f - (float)(n & 0x7fffffffu) / 1073741824.f;
}

inline float valueNoise3D(float x, float y, float z)
{
	int ix = (int)floor(x);
	int iy = (int)floor(y);
	int iz = (int)floor(z);
	float fx = x - (float)ix;
	float fy = y - (float)iy;
	float fz = z - (float)iz;

	float ux = fx * fx * (3.f - 2.f * fx);
	float uy = fy * fy * (3.f - 2.f * fy);
	float uz = fz * fz * (3.f - 2.f * fz);

	unsigned int h000 = (ix + 0) * 1619 + (iy + 0) * 31337 + (iz + 0) * 6271;
	unsigned int h100 = (ix + 1) * 1619 + (iy + 0) * 31337 + (iz + 0) * 6271;
	unsigned int h010 = (ix + 0) * 1619 + (iy + 1) * 31337 + (iz + 0) * 6271;
	unsigned int h110 = (ix + 1) * 1619 + (iy + 1) * 31337 + (iz + 0) * 6271;
	unsigned int h001 = (ix + 0) * 1619 + (iy + 0) * 31337 + (iz + 1) * 6271;
	unsigned int h101 = (ix + 1) * 1619 + (iy + 0) * 31337 + (iz + 1) * 6271;
	unsigned int h011 = (ix + 0) * 1619 + (iy + 1) * 31337 + (iz + 1) * 6271;
	unsigned int h111 = (ix + 1) * 1619 + (iy + 1) * 31337 + (iz + 1) * 6271;

	float v000 = hash(h000); float v100 = hash(h100);
	float v010 = hash(h010); float v110 = hash(h110);
	float v001 = hash(h001); float v101 = hash(h101);
	float v011 = hash(h011); float v111 = hash(h111);

	return v000 + ux*(v100-v000)
		+ uy*(v010-v000)
		+ uz*(v001-v000)
		+ ux*uy*(v000-v100-v010+v110)
		+ uy*uz*(v000-v010-v001+v011)
		+ ux*uz*(v000-v100-v001+v101)
		+ ux*uy*uz*(-v000+v100+v010-v110+v001-v101-v011+v111);
}

inline float3 compute_turbulence(float3 pos, float time, float strength, float freq)
{
    float ax = valueNoise3D(pos.x * freq + 0.0f,  pos.y * freq + 17.3f, time * freq);
    float ay = valueNoise3D(pos.x * freq + 53.1f, pos.y * freq + 0.0f,  time * freq + 5.7f);
    float az = valueNoise3D(pos.x * freq + 91.7f, pos.z * freq + 33.2f, time * freq + 11.3f);
    return (float3)(ax * strength, ay * strength, az * strength);
}

float3 computeVel(float3 pos, float3 gCenter, unsigned int seed)
{
	float3 dir = pos - gCenter;
	float dlen = length(dir);

	if (dlen > 1e-5f)
		dir = dir * (1.f / dlen);

	float3 up = (fabs(dir.y) < 0.9f) ? (float3)(0.f, 1.f, 0.f) : (float3)(1.f, 0.f, 0.f);
	float3 tangent = (float3)(
		dir.y * up.z - dir.z * up.y,
		dir.z * up.x - dir.x * up.z,
		dir.x * up.y - dir.y * up.x
	);

	float tlen = length(tangent);
	if (tlen > 1e-5f)
		tangent = tangent * (1.f / tlen);

	float speed = 0.3f + extract_random(&seed) * 0.4f; // % particle number

	return tangent * speed;	
}

void initCube(__private float3* pos, __private float3* color, __private GpuPhysicalParticle* physics, GpuSimulationState state, int id)
{
	unsigned int seed = id;
	*pos = (float3)(
				(extract_random(&seed) * 2.f - 1.f) * 0.5f,
				(extract_random(&seed) * 2.f - 1.f) * 0.5f,
				(extract_random(&seed) * 2.f - 1.f) * 0.5f
	);
	physics->vel.xyz = computeVel(*pos, state.gCenter.xyz, seed);
	physics->mass = 1.f;
	float3 gdir = state.gCenter.xyz - *pos;
	float gdist = length(gdir);
	physics->maxLife = 2.f + extract_random(&seed) * 4.f;
	physics->life = physics->maxLife;
	physics->alive = 1;
	*color = (float3)(gdist, 0.f, physics->maxLife - physics->life);
}

void initSphere(__private float3* pos, __private float3* color, __private GpuPhysicalParticle* physics, GpuSimulationState state, int id)
{
	unsigned int seed = id;
	float u = extract_random(&seed);
	float v = extract_random(&seed);
	float w = extract_random(&seed);

	float theta = u * 2.0f * M_PI_F;
	float phi = acos(2.0f * v - 1.0f);
	float r = cbrt(w) * 0.5f;

	*pos = (float3)(
			r * sin(phi) * cos(theta),
			r * sin(phi) * sin(theta),
			r * cos(phi)
	);

	physics->vel.xyz = computeVel(*pos, state.gCenter.xyz, seed);
	physics->mass = 1.f;
	float3 gdir = state.gCenter.xyz - *pos;
	float gdist = length(gdir);
	physics->maxLife = 2.f + extract_random(&seed) * 4.f;
	physics->life = physics->maxLife;
	physics->alive = 1;
	*color = (float3)(gdist, 0.f, physics->maxLife - physics->life);
}

/* Kernel */

__kernel void update_particles(__global float* vbo_data, 
								__global GpuPhysicalParticle* physics,
								const GpuSimulationState state
                               ) 
{
	int id = get_global_id(0);

	const float G = (state.gravity != 0 ? state.G : 0.f);
	const float softening = 0.5f;
	const float dampling = 0.995f;
	const float turbulenceStrength = 0.4f;
	const float turbulenceFreq = 1.5f;
	int vbo_index = id * 6;
	float3 pos = (float3)(vbo_data[vbo_index], vbo_data[vbo_index + 1], vbo_data[vbo_index + 2]);
	float3 color = (float3)(vbo_data[vbo_index + 3], vbo_data[vbo_index + 5], vbo_data[vbo_index + 5]);
	float3 vel = physics[id].vel.xyz;
	float life = physics[id].life;
	float maxLife = physics[id].maxLife;
	GpuPhysicalParticle tmp = physics[id];

	if (state.cube == 1)
	{
		initCube(&pos, &color, &tmp, state, id);
		physics[id] = tmp;
	}

	if (state.sphere == 1)
	{
		initSphere(&pos, &color, &tmp, state, id);
		physics[id] = tmp;
	}

	if (!physics[id].alive)
	{
		vbo_data[vbo_index] = 99999.f;
		return;
	}

	if (!state.immortal)
		life -= state.deltaTime;
	if (life <= 0.f)
	{
		physics[id].alive = 0;
		vbo_data[vbo_index] = 99999.f;
		return;
	}

	float3 dir = state.gCenter.xyz - pos;
	float gDist = length(dir);
	if (gDist > 1e-5f)
	{
		float magnitude = G / (gDist * gDist + softening * softening);
		vel += (dir / gDist) * magnitude * state.deltaTime;
	}

	if (state.turbulence == 1)
	{
		float3 turb = compute_turbulence(pos, state.time, turbulenceStrength, turbulenceFreq);
		vel += turb * state.deltaTime;
	}

	pos += vel * state.deltaTime;
	vel *= dampling;

	float t = life / maxLife;
	float center_dist = 1.0f - gDist / (gDist + 1.0f);
	center_dist = center_dist * center_dist;
	
	color.x = t + center_dist;
	color.y = 0.0f + center_dist;
	color.z = (1.0f - t) + center_dist;

	vbo_data[vbo_index + 0] = pos.x;
	vbo_data[vbo_index + 1] = pos.y;
	vbo_data[vbo_index + 2] = pos.z;
	vbo_data[vbo_index + 3] = color.x;
	vbo_data[vbo_index + 4] = color.y;
	vbo_data[vbo_index + 5] = color.z;

	physics[id].vel.xyz = vel;
	physics[id].life = life;
}