#pragma once

#include "Particle.h"

class ParticleSystem
{
public:
	int numActiveParticles = 0;

	int spawnLimit;
	float spawnTimer = 0.0f;
	float spawnRate = 1.0f;
	int spawnAmount = 1;

	std::string systemName;
	Vector3D position;

	bool isActive;

	//Particle** arrayParticles;
	Particle* arrayParticles[50];
	Particle* spawnTemplate = nullptr;

	ParticleSystem();
	ParticleSystem(int particleLimit, Vector3D systemLocation, std::string name);
	~ParticleSystem();

	void Update(float deltaTime, float yaw, float pitch);
	void Draw(ID3D11DeviceContext* pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* pConstantBuffer);

	void SpawnParticles(Vector3D pos);
	Particle* GetParticleTemplate() const { return spawnTemplate; }

	virtual void UpdateParticles(Particle* particle, float deltaTime) = 0;
	virtual void SpawnChanges(Particle* particle) = 0;

	void SetParticleTemplate(Particle* newTemplate) { spawnTemplate = newTemplate; }
	void SetMesh(MeshData mesh) { spawnTemplate->meshData = mesh; }
	void SetSpawnRate(float rate) { spawnRate = rate; }

	void Shutdown();
};