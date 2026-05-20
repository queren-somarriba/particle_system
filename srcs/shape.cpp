#include "particle_system.hpp"
#include "utils.hpp"

namespace
{
	void initVel(Particle& p, const vect4f& gCenter)
	{
		vect4f dir = p.pos - gCenter;
		float dlen = dir.length();

		if (dlen > 1e-5f)
			dir = dir * (1.f / dlen);

		vect4f up = (std::abs(dir.y) < 0.9f) ? vect4f(0.f, 1.f, 0.f) : vect4f(1.f, 0.f, 0.f);
		vect4f tangent = vect4f(
			dir.y * up.z - dir.z * up.y,
			dir.z * up.x - dir.x * up.z,
			dir.x * up.y - dir.y * up.x
		);

		float tlen = tangent.length();
		if (tlen > 1e-5f)
			tangent = tangent * (1.f / tlen);

		float speed = 0.3f + randf() * 0.4f;
		p.vel = tangent * speed;
	}
}

void initCube(std::vector<Particle>& particles, unsigned int n, const vect4f& gCenter)
{
	for (unsigned int i = 0; i < n; ++i)
	{
		Particle p;
		p.pos = vect4f(
			(randf() * 2.f - 1.f) * 0.5f,
			(randf() * 2.f - 1.f) * 0.5f,
			(randf() * 2.f - 1.f) * 0.5f
		);
		initVel(p, gCenter);
		p.mass = 1.f;
		vect4f gdir = gCenter - p.pos;
		p.gDist = gdir.length();
		p.color = vect4f(p.gDist, 0.f, p.maxLife - p.life);
		p.maxLife = 2.f + randf() * 4.f;
		p.life = p.maxLife;
		p.alive = true;
		particles[i] = p;
	}
}

void initSphere(std::vector<Particle>& particles, unsigned int n, const vect4f& gCenter)
{
	for (unsigned int i = 0; i < n; ++i)
	{
		Particle p;
		vect4f pos;
		do {
			pos = vect4f(
				(randf() * 2.f - 1.f) * 0.5f,
				(randf() * 2.f - 1.f) * 0.5f,
				(randf() * 2.f - 1.f) * 0.5f
			);
		}
		while (pos.length() > 0.5f);

		p.pos = pos;
		initVel(p, gCenter);
		p.mass = 1.f;
		vect4f gdir = gCenter - p.pos;
		p.gDist = gdir.length();
		p.color = vect4f(p.gDist, 0.f, p.maxLife - p.life);
		p.maxLife = 2.f + randf() * 4.f;
		p.life = p.maxLife;
		p.alive = true;
		particles[i] = p;
	}
}

void resetParticles(psData& data)
{
	if (data.state.shape == CUBE)
		initCube(data.particles, data.particle_number, data.state.gCenter);
	else
		initSphere(data.particles, data.particle_number, data.state.gCenter);

	std::vector<float> gpuVector;
	setupGpuData(data, gpuVector);
}