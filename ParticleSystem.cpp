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
	for (size_t i = 0; i < spawnLimit; i++)
	{
		arrayParticles[i] = new Particle();
	}
}

void ParticleSystem::Update(float deltaTime)
{
	for (size_t i = 0; i < spawnLimit; i++)
	{
		if (arrayParticles[i]->isActive)
			arrayParticles[i]->Update(deltaTime);
	}
	UpdateParticles(nullptr, deltaTime);
}

void ParticleSystem::Draw(ID3D11DeviceContext* pImmediateContext, ConstantBuffer cb, ID3D11Buffer* pConstantBuffer)
{
	for (size_t i = 0; i < spawnLimit; i++)
	{
		if (arrayParticles[i]->isActive)
		{
			pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

			XMFLOAT3 tempPos = { arrayParticles[i]->transform.GetPos().x, arrayParticles[i]->transform.GetPos().y, arrayParticles[i]->transform.GetPos().z };
			XMFLOAT3 tempScale = { arrayParticles[i]->transform.GetScale().x, arrayParticles[i]->transform.GetScale().y, arrayParticles[i]->transform.GetScale().z };
			/*if (appearance->GetBillboard())
				tempRot.y += yaw + XM_PI;*/

			if(arrayParticles[i]->isFullbright)
				cb.AmbientMtrl = { arrayParticles[i]->colour.x, arrayParticles[i]->colour.y, arrayParticles[i]->colour.z, 0.0f };
			else
				cb.DiffuseMtrl = { arrayParticles[i]->colour.x, arrayParticles[i]->colour.y, arrayParticles[i]->colour.z, 0.0f };

			pImmediateContext->PSSetShaderResources(0, 1, &arrayParticles[i]->texture);

			pImmediateContext->IASetVertexBuffers(0, 1, &arrayParticles[i]->meshData.VertexBuffer, &arrayParticles[i]->meshData.VBStride, &arrayParticles[i]->meshData.VBOffset);
			pImmediateContext->IASetIndexBuffer(arrayParticles[i]->meshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

			XMMATRIX posMatrix = XMMatrixTranslation(tempPos.x, tempPos.y, tempPos.z);
			//XMMATRIX rotMatrix = XMMatrixRotationX(tempRot.x) * XMMatrixRotationY(tempRot.y) * XMMatrixRotationZ(tempRot.z);
			XMMATRIX scaleMatrix = XMMatrixScaling(tempScale.x, tempScale.y, tempScale.z);
			//XMMATRIX temp = XMMatrixMultiply(XMMatrixMultiply(rotMatrix, scaleMatrix), posMatrix);
			XMMATRIX temp = XMMatrixMultiply(scaleMatrix, posMatrix);
			XMFLOAT4X4 tempWorld;
			XMStoreFloat4x4(&tempWorld, temp);
			cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&tempWorld));

			pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
			pImmediateContext->DrawIndexed(arrayParticles[i]->meshData.IndexCount, 0, 0);
		}     
	}
}

void ParticleSystem::SpawnParticles(Vector3D pos)
{
	for (size_t i = 0; i < spawnLimit; i++)
	{
		if (!arrayParticles[i]->isActive)
		{
			arrayParticles[i]->transform = spawnTemplate->transform;
			arrayParticles[i]->colour = spawnTemplate->colour;
			arrayParticles[i]->lifetime = spawnTemplate->lifetime;
			arrayParticles[i]->meshData = spawnTemplate->meshData;
			arrayParticles[i]->texture = spawnTemplate->texture;
			arrayParticles[i]->isFullbright = spawnTemplate->isFullbright;

			arrayParticles[i]->particleModel.SetTransform(*spawnTemplate->particleModel.GetTransform());
			arrayParticles[i]->particleModel.SetVelocity(spawnTemplate->particleModel.GetVelocity());
			arrayParticles[i]->particleModel.SetThrust(spawnTemplate->particleModel.GetThrust());
			arrayParticles[i]->particleModel.SetFriction(spawnTemplate->particleModel.GetFriction());
			arrayParticles[i]->particleModel.SetLaminarFlow(true);
			arrayParticles[i]->particleModel.SetMass(spawnTemplate->particleModel.GetMass());
			arrayParticles[i]->transform.SetPos(pos);

			arrayParticles[i]->isActive = true;
			spawnTimer = 0.0f;
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