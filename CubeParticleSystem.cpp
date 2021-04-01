#include "CubeParticleSystem.h"

CubeParticleSystem::CubeParticleSystem(MeshData mesh, ID3D11ShaderResourceView* tex) : ParticleSystem(30, position, "CubeParticle")
{
	spawnRate = 0.2;
	spawnAmount = 1;

	Particle* CubeParticleTemplate = new Particle( { 0.2, 0.8, 0.8 }, 2.0f, 2.0f, mesh, tex, false, false );

	CubeParticleTemplate->particleModel->GetTransform()->SetScale({ 0.5, 0.5, 0.5 });
	CubeParticleTemplate->particleModel->SetVelocity({ 0, 0, 0 });
	CubeParticleTemplate->particleModel->SetThrust({ 0, 0, 0 });
	CubeParticleTemplate->particleModel->SetLaminarFlow(true);

	SetParticleTemplate(CubeParticleTemplate);
}

CubeParticleSystem::CubeParticleSystem()
{

}

void CubeParticleSystem::UpdateParticles(Particle* particle, float deltatime)
{
	spawnTimer += deltatime;
	if (spawnTimer >= spawnRate)
	{
		for (size_t i = 0; i < spawnAmount; ++i)
		{
			SpawnParticles({ (rand() % 40 - 20.0f) / 10.0f + position.x, position.y + 8.0f, (rand() % 40 - 20.0f) / 10.0f + position.z });
		}
	}
}

void CubeParticleSystem::SpawnChanges(Particle* particle)
{

}