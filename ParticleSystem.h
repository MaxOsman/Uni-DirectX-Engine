#pragma once

#include "Particle.h"

class ParticleSystem
{
public:
	int numParticles;
	int numActiveParticles;
	int spawnLimit;

	float spawnTimer;
	float spawnRate = 1;	//Per sec

	Particle** arrayParticles;
	Particle* spawnTemplate = nullptr;

	std::string systemName;

	//Emmitter* spawnEmmiter = nullptr;
	int spawnAmount = 1;

	ParticleSystem(int particleLimit, Vector3D systemLocation, std::string name);
	void Initialise();

	void Update(float deltaTime);
	void Draw(ID3D11DeviceContext* pImmediateContext, ConstantBuffer cb);

	void SpawnParticles(Vector3D pos);
	void DeactivateParticle(int pos);
	Particle* GetParticleTemplate() const { return spawnTemplate; }

	virtual void UpdateParticles(Particle* particle, float deltaTime) = 0;
	virtual void SpawnChanges(Particle* particle) = 0;

	static void Gravity(Particle* particle, Vector3D direction, float force);
	static void Drag(Particle* particle, float K1, float K2);
	static void Wind(Particle* particle, Vector3D direction, float windForce);
	static void Thrust(Particle* particle, Vector3D direction, float thrustForce);
	static void Repulsion(Particle* particle, Vector3D point, float minDistance, float maxDIstance, float force);

	void SetParticleTemplate(Particle* newTemplate);
	void SetSpawnRate(float rate) { spawnRate = rate; }

	void Shutdown();
};