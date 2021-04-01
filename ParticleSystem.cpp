#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(int particleLimit, Vector3D systemLocation, std::string name)
{
	spawnLimit = particleLimit;
	position = systemLocation;
	systemName = name;
	//arrayParticles = new Particle * [spawnLimit];
}

ParticleSystem::ParticleSystem()
{

}

ParticleSystem::~ParticleSystem()
{
	delete[] arrayParticles;
}

void ParticleSystem::Initialise()
{
	for (size_t i = 0; i < spawnLimit; ++i)
	{
		arrayParticles[i] = new Particle();
	}
}

void ParticleSystem::Update(float deltaTime, float yaw, float pitch)
{
	for (size_t i = 0; i < spawnLimit; ++i)
	{
		if (arrayParticles[i]->isActive)
		{
			arrayParticles[i]->Update(deltaTime, yaw, pitch);
			if (arrayParticles[i]->lifetime < 0.0f)
			{
				// Particle is dead
				arrayParticles[i]->isActive = false;
			}
		}	
	}
	UpdateParticles(nullptr, deltaTime);
}

void ParticleSystem::Draw(ID3D11DeviceContext* pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* pConstantBuffer)
{
	for (size_t i = 0; i < spawnLimit; ++i)
	{
		if (arrayParticles[i]->isActive)
		{
			arrayParticles[i]->Draw(pImmediateContext, cb, pConstantBuffer);
		}     
	}
}

void ParticleSystem::SpawnParticles(Vector3D pos)
{
	for (size_t i = 0; i < spawnLimit; ++i)
	{
		if (!arrayParticles[i]->isActive)
		{
			arrayParticles[i]->colour = spawnTemplate->colour;
			arrayParticles[i]->lifetime = spawnTemplate->lifetime;
			arrayParticles[i]->meshData = spawnTemplate->meshData;
			arrayParticles[i]->texture = spawnTemplate->texture;
			arrayParticles[i]->isFullbright = spawnTemplate->isFullbright;

			arrayParticles[i]->particleModel->SetTransform(new Transform());
			arrayParticles[i]->particleModel->SetVelocity(spawnTemplate->particleModel->GetVelocity());
			arrayParticles[i]->particleModel->SetThrust(spawnTemplate->particleModel->GetThrust());
			arrayParticles[i]->particleModel->SetFriction(spawnTemplate->particleModel->GetFriction());
			arrayParticles[i]->particleModel->SetLaminarFlow(spawnTemplate->particleModel->GetLaminarFlow());
			arrayParticles[i]->particleModel->SetMass(spawnTemplate->particleModel->GetMass());

			arrayParticles[i]->particleModel->GetTransform()->SetPos(pos);
			arrayParticles[i]->particleModel->GetTransform()->SetOri(spawnTemplate->particleModel->GetTransform()->GetOri());
			arrayParticles[i]->particleModel->GetTransform()->SetScale(spawnTemplate->particleModel->GetTransform()->GetScale());
			arrayParticles[i]->particleModel->GetTransform()->SetBillboard(spawnTemplate->particleModel->GetTransform()->GetBillboard());

			arrayParticles[i]->isActive = true;
			spawnTimer = 0.0f;

			SpawnChanges(arrayParticles[i]);

			break;
		}		
	}
}

void ParticleSystem::Shutdown()
{
	for (size_t i = 0; i < spawnLimit; i++)
	{
		arrayParticles[i] = nullptr;
	}
	//arrayParticles = nullptr;
}