#pragma once

#include "ParticleSystem.h"

class CubeParticleSystem : public ParticleSystem
{
public:
	void UpdateParticles(Particle* particle, float deltatime) override;
	void SpawnChanges(Particle* particle) override;

	CubeParticleSystem(MeshData mesh, ID3D11ShaderResourceView* tex);
	CubeParticleSystem();
};