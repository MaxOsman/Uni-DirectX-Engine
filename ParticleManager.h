#pragma once

#include "ParticleSystem.h"

class ParticleManager
{
public:
	ParticleManager(int maxParticleSystems);
	~ParticleManager();

	ParticleSystem** particleSystems;
	int particleSystemCount = 0;

	void Initialise();
	void AddSystem(ParticleSystem* newSystem);

	void RemoveSystem(std::string systemName);
	void RemoveSystem(int index);

	void Shutdown();
	bool DoesExist(std::string systemName);

	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* pImmediateContext, ConstantBuffer cb);
};