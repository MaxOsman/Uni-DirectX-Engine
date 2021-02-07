#include "ParticleSystem.h"

void ParticleSystem::Initialise()
{
	arrayParticles = new Particle * [numParticles];

	for (size_t i = 0; i < numParticles; i++)
	{
		arrayParticles[i] = new Particle(Vector3D(0, 0, 0), Vector3D(0, 0, 0), Vector3D(0, 0, 0), 0, 0);
	}
}