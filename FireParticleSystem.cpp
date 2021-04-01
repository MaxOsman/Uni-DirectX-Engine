#include "FireParticleSystem.h"

FireParticleSystem::FireParticleSystem(MeshData mesh, ID3D11ShaderResourceView* tex) : ParticleSystem(50, position, "FireParticle")
{
	spawnRate = 0.02;
	spawnAmount = 1;

	Particle* fireParticleTemplate = new Particle( { 1, 0.2, 0 }, 1.0f, 0.5f, mesh, tex, true, true );

	fireParticleTemplate->particleModel->GetTransform()->SetScale({ 0.25, 0.25, 0.25 });
	fireParticleTemplate->particleModel->SetVelocity({ 0, 20, 0 });
	fireParticleTemplate->particleModel->SetThrust({ 0, 0, 0 });
	fireParticleTemplate->particleModel->SetLaminarFlow(false);

	SetParticleTemplate(fireParticleTemplate);
}

FireParticleSystem::FireParticleSystem()
{

}

void FireParticleSystem::UpdateParticles(Particle* particle, float deltatime)
{
	spawnTimer += deltatime;
	if (spawnTimer >= spawnRate)
	{
		for (size_t i = 0; i < spawnAmount; ++i)
		{
			SpawnParticles({ (rand() % 40 - 20.0f) / 20.0f + position.x, position.y + 1.0f, (rand() % 40 - 20.0f) / 20.0f + position.z });
		}
	}
}

void FireParticleSystem::SpawnChanges(Particle* particle)
{

}