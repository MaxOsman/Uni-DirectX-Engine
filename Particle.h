#pragma once

#include "ParticleModel.h"

class Particle
{
public:
	Particle(Vector3D initialPosition, Vector3D velocity, Vector3D clr, float mass, float life);
	~Particle();

	Transform transform;
	ParticleModel particleModel;
	Vector3D colour;

	bool isActive = false;
	float lifetime = 0;

	void Update(float deltaTime);
	void SetActive(bool active) { isActive = active; }
};