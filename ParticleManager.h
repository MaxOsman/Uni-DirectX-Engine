#pragma once

#include "ParticleSystem.h"
#include "FireParticleSystem.h"
#include <vector>

class ParticleManager
{
public:
	ParticleManager(int maxParticleSystems);
	~ParticleManager();

	ParticleSystem** particleSystems;

	int particleSystemCount = 0;
	int maxSystemCount;

	void Initialise();
	void AddSystem(ParticleSystem* newSystem);

	void RemoveSystem(std::string systemName);
	void RemoveSystem(int index);

	void Shutdown();
	bool DoesExist(std::string systemName);

	void Update(float deltaTime, float yaw, float pitch);
	void Render(ID3D11DeviceContext* pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* pConstantBuffer);
};