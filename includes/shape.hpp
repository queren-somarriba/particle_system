#pragma once

#include "particle_system.hpp"

void initCube(std::vector<Particle>& particles, unsigned int n, const vect4f& gCenter);


void initSphere(std::vector<Particle>& particles, unsigned int n, const vect4f& gCenter);

void resetParticles(psData& data);