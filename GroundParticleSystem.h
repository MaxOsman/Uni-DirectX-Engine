#pragma once

#include "ParticleSystem.h"

class GroundParticleSystem : public ParticleSystem
{
public:
	void UpdateParticles(Particle* particle, float deltatime) override;
	void SpawnChanges(Particle* particle) override;

	GroundParticleSystem(MeshData mesh, ID3D11ShaderResourceView* tex, Vector3D pos);
	GroundParticleSystem();
};