#pragma once

#include "ParticleModel.h"

class Particle
{
public:
	Particle(Vector3D clr, float mass, float life, MeshData mesh, ID3D11ShaderResourceView* tex, bool bright);
	Particle();
	~Particle();

	Transform transform;
	ParticleModel particleModel;
	Vector3D colour;
	MeshData meshData;
	ID3D11ShaderResourceView* texture;
	bool isActive;
	float lifetime;
	bool isFullbright;

	void Update(float deltaTime);
};