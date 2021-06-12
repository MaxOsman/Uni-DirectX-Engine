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

	void AddSystem(ParticleSystem* newSystem);
	void RemoveSystem(std::string sysName);
	void RemoveSystem(int index) { particleSystems[index] = nullptr; };

	bool DoesExist(std::string sysName);

	void Update(float deltaTime, float yaw, float pitch);
	void Render(ID3D11DeviceContext* pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* pConstantBuffer);
};