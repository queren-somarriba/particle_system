#include "ParticleSystem.hpp"
#include <random>

ParticleSystem::ParticleSystem()
{
	this->_particlePool.resize(1000);
}

void ParticleSystem::onUpdate()