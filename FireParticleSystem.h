#pragma once

#include "ParticleSystem.h"

class FireParticleSystem : public ParticleSystem
{
public:
	void UpdateParticles(Particle* particle, float deltatime) override;
	void SpawnChanges(Particle* particle) override;

	FireParticleSystem(MeshData mesh, ID3D11ShaderResourceView* tex);
	FireParticleSystem();
};