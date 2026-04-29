/* #pragma once

#include "vect4f.hpp"
#include <memory>
#include "shader.hpp"

struct ParticleProps
{
	vect2f position;
	vect2f velocity, velocityVariation;
	vect4f startColor, endColor;
	float startSize, endSize, sizeVariation;
	float lifeTime = 1.f;
};

class ParticleSystem
{
	public:
				ParticleSystem();
		void	onUpdate();
		void	onRender();
		void	emit(const ParticleProps& particleProps);

	private:
		struct Particle
		{
			vect2f	position;
			vect2f	velocity;
			vect4f	startColor, endColor;
			float	rotation = 0.f;
			float	startSize, endSize;
			float	lifeTime = 1.f;
			float	lifeRemaining = 0.f;
			bool	active = false;
		};
		std::vector<Particle>	_ParticlePool;
		uint32_t				poolIndex = 999;
		GLuint					_quadVA = 0;
		std::unique_ptr<Shader>	_particleShader;
		GLuint					_particleShaderTransform, _particleShaderColor;
}; */