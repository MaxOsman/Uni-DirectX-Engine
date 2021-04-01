#pragma once

#include "Particle.h"

class ParticleSystem
{
public:
	int numActiveParticles = 0;

	int spawnLimit;
	float spawnTimer = 0.0f;
	float spawnRate = 1.0f;

	//Particle** arrayParticles;
	Particle* arrayParticles[50];
	Particle* spawnTemplate = nullptr;

	std::string systemName;
	Vector3D position;

	int spawnAmount = 1;

	ParticleSystem();
	ParticleSystem(int particleLimit, Vector3D systemLocation, std::string name);
	~ParticleSystem();
	void Initialise();

	void Update(float deltaTime, float yaw, float pitch);
	void Draw(ID3D11DeviceContext* pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* pConstantBuffer);

	void SpawnParticles(Vector3D pos);
	//void DeactivateParticle(int i);
	Particle* GetParticleTemplate() const { return spawnTemplate; }

	virtual void UpdateParticles(Particle* particle, float deltaTime) = 0;
	virtual void SpawnChanges(Particle* particle) = 0;

	static void Gravity(Particle* particle, Vector3D direction, float force);
	static void Drag(Particle* particle, float K1, float K2);
	static void Wind(Particle* particle, Vector3D direction, float windForce);
	static void Thrust(Particle* particle, Vector3D direction, float thrustForce);
	static void Repulsion(Particle* particle, Vector3D point, float minDistance, float maxDistance, float force);

	void SetParticleTemplate(Particle* newTemplate) { spawnTemplate = newTemplate; }
	void SetMesh(MeshData mesh) { spawnTemplate->meshData = mesh; }
	void SetSpawnRate(float rate) { spawnRate = rate; }

	void Shutdown();
};