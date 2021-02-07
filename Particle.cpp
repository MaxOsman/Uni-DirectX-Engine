#include "Particle.h"

Particle::Particle(Vector3D initialPosition, Vector3D velocity, Vector3D clr, float mass, float life)
{
	transform.SetPos(initialPosition);
	particleModel = {&transform, velocity, mass};
	colour = clr;
}

void Particle::Update(float deltaTime)
{
	particleModel.Update(deltaTime);
}