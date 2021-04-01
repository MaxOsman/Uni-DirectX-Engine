#include "SmokeParticleSystem.h"

SmokeParticleSystem::SmokeParticleSystem(MeshData mesh, ID3D11ShaderResourceView* tex) : ParticleSystem(50, position, "SmokeParticle")
{
	spawnRate = 0.1;
	spawnAmount = 1;

	Particle* SmokeParticleTemplate = new Particle( { 0.6, 0.6, 0.6 }, 0.5f, 4.0f, mesh, tex, true, true );

	SmokeParticleTemplate->particleModel->GetTransform()->SetScale({ 0.6, 0.6, 0.6 });
	SmokeParticleTemplate->particleModel->SetVelocity({ 0, 10, 0 });
	SmokeParticleTemplate->particleModel->SetThrust({ 0, G * 1.2, 0 });
	SmokeParticleTemplate->particleModel->SetLaminarFlow(false);

	SetParticleTemplate(SmokeParticleTemplate);
}

SmokeParticleSystem::SmokeParticleSystem()
{

}

void SmokeParticleSystem::UpdateParticles(Particle* particle, float deltatime)
{
	spawnTimer += deltatime;
	if (spawnTimer >= spawnRate)
	{
		for (size_t i = 0; i < spawnAmount; ++i)
		{
			SpawnParticles({ position.x, position.y + 1.0f, position.z });
		}
	}
}

void SmokeParticleSystem::SpawnChanges(Particle* particle)
{
	particle->particleModel->SetVelocity({rand() % 24 - 12.0f, 10, rand() % 24 - 12.0f });
}