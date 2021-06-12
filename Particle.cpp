#include "Particle.h"

Particle::Particle(Vector3D clr, float mass, float life, MeshData mesh, ID3D11ShaderResourceView* tex, bool bright, bool bill)
{
	colour = clr;
	lifetime = life;
	meshData = mesh;
	texture = tex;
	isActive = false;
	isFullbright = bright;
	particleModel = new ParticleModel( new Transform(), mass );
	particleModel->GetTransform()->SetBillboard(bill);
}

Particle::Particle()
{
	isActive = false;
	lifetime = 0;
	particleModel = new ParticleModel();
}

Particle::~Particle()
{
	particleModel = nullptr;
	texture = nullptr;
}

void Particle::Update(float deltaTime, float yaw, float pitch)
{
	particleModel->GetTransform()->Update(yaw, pitch);
	particleModel->Update(deltaTime);

	lifetime -= deltaTime;
}

void Particle::Draw(ID3D11DeviceContext* pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* pConstantBuffer)
{
	pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

	if (isFullbright)
	{
		cb->DiffuseMtrl = { 0.0f, 0.0f, 0.0f, 0.0f };
		cb->AmbientMtrl = { colour.x * 5, colour.y * 5 , colour.z * 5, 0.0f };
	}	
	else
	{
		cb->DiffuseMtrl = { colour.x, colour.y, colour.z, 0.0f };
		cb->AmbientMtrl = { 0.0f, 0.0f, 0.0f, 0.0f };
	}
	cb->SpecularMtrl = { 0.0f, 0.0f, 0.0f, 0.0f };

	pImmediateContext->PSSetShaderResources(0, 1, &texture);

	pImmediateContext->IASetVertexBuffers(0, 1, &meshData.VertexBuffer, &meshData.VBStride, &meshData.VBOffset);
	pImmediateContext->IASetIndexBuffer(meshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	particleModel->GetTransform()->Render(cb);

	pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, cb, 0, 0);
	pImmediateContext->DrawIndexed(meshData.IndexCount, 0, 0);
}