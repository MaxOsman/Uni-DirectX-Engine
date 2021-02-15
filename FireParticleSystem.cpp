#include "FireParticleSystem.h"

FireParticleSystem::FireParticleSystem(MeshData mesh, ID3D11ShaderResourceView* tex) : ParticleSystem(50, position, "FireParticle")
{
	spawnRate = 1;
	spawnAmount = 10;

	Particle* fireParticleTemplate = new Particle(Vector3D(1, 1, 1), 1.0f, 0.5f, mesh, tex, true);

	fireParticleTemplate->particleModel.GetTransform()->SetScale({ 0.25, 0.25, 0.25 });
	fireParticleTemplate->particleModel.SetVelocity(Vector3D{ 0, 10, 0 });
	fireParticleTemplate->particleModel.SetThrust(Vector3D{ 0, G * fireParticleTemplate->particleModel.GetMass(), 0 });
	fireParticleTemplate->particleModel.SetFriction(Vector3D{ 0, 0, 0 });
	fireParticleTemplate->particleModel.SetLaminarFlow(false);

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
		for (size_t i = 0; i < spawnAmount; i++)
		{
			SpawnParticles({ (rand() % 30 - 15.0f) / 15.0f, 1.0f, (rand() % 30 - 15.0f) / 15.0f });
		}
	}
}

void FireParticleSystem::SpawnChanges(Particle* particle)
{

}