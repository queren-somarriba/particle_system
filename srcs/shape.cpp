#include "particle_system.hpp"
#include "utils.hpp"


void initCube(std::vector<Particle>& particles, unsigned int n, const vect4f& gCenter)
{
	for (unsigned int i = 0; i < n; ++i)
	{
		Particle p;
		p.pos = vect4f(
			randf() * 2.f - 1.f,
			randf() * 2.f - 1.f,
			randf() * 2.f - 1.f
		);
		p.vel = vect4f(0.f, 0.f, 0.f);
		p.mass = 1.f;
		vect4f gdir = gCenter - p.pos;
		p.gDist = gdir.length();
		p.color = vect4f(p.gDist, p.gDist * 0.5f, 0.f);
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
				randf() * 2.f - 1.f,
				randf() * 2.f - 1.f,
				randf() * 2.f - 1.f
			);
		}
		while (pos.length() > 1.f);

		p.pos = pos;
		p.vel = vect4f(0.f, 0.f, 0.f);
		p.mass = 1.f;
		vect4f gdir = gCenter - p.pos;
		p.gDist = gdir.length();
		p.color = vect4f(p.gDist, p.gDist * 0.5f, 0.f);
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
	gpuVector.reserve(data.particle_number * 6);
	for (const Particle& p : data.particles)
	{
		gpuVector.push_back(p.pos.x);
		gpuVector.push_back(p.pos.y);
		gpuVector.push_back(p.pos.z);
		gpuVector.push_back(p.color.x);
		gpuVector.push_back(p.color.y);
		gpuVector.push_back(p.color.z);
	}
	data.vbo->bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		gpuVector.size() * sizeof(float), gpuVector.data());
	data.vbo->unbind();
}