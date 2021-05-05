#include "GroundParticleSystem.h"

GroundParticleSystem::GroundParticleSystem(MeshData mesh, ID3D11ShaderResourceView* tex, Vector3D pos) : ParticleSystem(10, position, "GroundParticle")
{
	spawnRate = 1.0;
	spawnAmount = 10;
	position = pos;

	Particle* GroundParticleTemplate = new Particle( { 0.8, 0.6, 0.4 }, 0.5f, 0.75f, mesh, tex, true, true );

	GroundParticleTemplate->particleModel->GetTransform()->SetScale({ 0.5, 0.5, 0.5 });
	GroundParticleTemplate->particleModel->SetVelocity({ 0, 10, 0 });
	GroundParticleTemplate->particleModel->SetThrust({ 0, G * 1.2, 0 });
	GroundParticleTemplate->particleModel->SetLaminarFlow(false);

	SetParticleTemplate(GroundParticleTemplate);

	for (size_t i = 0; i < spawnAmount; ++i)
	{
		SpawnParticles({ position.x, position.y + 1.0f, position.z });
	}
}

GroundParticleSystem::GroundParticleSystem()
{

}

void GroundParticleSystem::UpdateParticles(Particle* particle, float deltatime)
{
	spawnTimer += deltatime;
	if (spawnTimer >= spawnRate)
	{
		isActive = false;
	}
}

void GroundParticleSystem::SpawnChanges(Particle* particle)
{
	particle->particleModel->SetVelocity({rand() % 24 - 12.0f, 0, rand() % 24 - 12.0f });
}