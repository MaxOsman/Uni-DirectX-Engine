#pragma once

#include "ParticleModel.h"

class Particle
{
public:
	Particle(Vector3D clr, float mass, float life, MeshData mesh, ID3D11ShaderResourceView* tex, bool bright, bool bill);
	Particle();
	~Particle();

	ParticleModel* particleModel;
	Vector3D colour;
	MeshData meshData;
	ID3D11ShaderResourceView* texture;
	bool isActive;
	float lifetime;
	bool isFullbright;

	void Update(float deltaTime, float yaw, float pitch);
	void Draw(ID3D11DeviceContext* pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* pConstantBuffer);
};