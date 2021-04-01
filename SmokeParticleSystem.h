#pragma once

#include "ParticleSystem.h"

class SmokeParticleSystem : public ParticleSystem
{
public:
	void UpdateParticles(Particle* particle, float deltatime) override;
	void SpawnChanges(Particle* particle) override;

	SmokeParticleSystem(MeshData mesh, ID3D11ShaderResourceView* tex);
	SmokeParticleSystem();
};