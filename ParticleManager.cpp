#include "ParticleManager.h"

ParticleManager::ParticleManager(int maxParticleSystems)
{
	maxSystemCount = maxParticleSystems;
	particleSystems = new ParticleSystem * [maxSystemCount];
	for (size_t i = 0; i < maxSystemCount; i++)
	{
		particleSystems[i] = nullptr;
	}
}

ParticleManager::~ParticleManager()
{
	delete[] particleSystems;
}

void ParticleManager::Initialise()
{
	
}

void ParticleManager::Update(float deltaTime, float yaw, float pitch)
{
	for (unsigned short i = 0; i < maxSystemCount; ++i)
	{
		if (particleSystems[i] != nullptr)
		{
			if (!particleSystems[i]->isActive)
			{
				particleSystems[i] = nullptr;
			}
			else
				particleSystems[i]->Update(deltaTime, yaw, pitch);
		}
	}
}

void ParticleManager::Render(ID3D11DeviceContext* pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* pConstantBuffer)
{
	for (unsigned short i = 0; i < maxSystemCount; ++i)
	{
		if (particleSystems[i] != nullptr)
			particleSystems[i]->Draw(pImmediateContext, cb, pConstantBuffer);
	}
}

void ParticleManager::AddSystem(ParticleSystem* newSystem)
{
	for (unsigned short i = 0; i < maxSystemCount; ++i)
	{
		if (particleSystems[i] == nullptr)
		{
			particleSystems[i] = newSystem;
			break;
		}
	}
}