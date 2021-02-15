#include "Particle.h"

Particle::Particle(Vector3D clr, float mass, float life, MeshData mesh, ID3D11ShaderResourceView* tex, bool bright)
{
	colour = clr;
	lifetime = life;
	meshData = mesh;
	texture = tex;
	isActive = false;
	isFullbright = bright;
	particleModel = { &transform, {0, 0, 0}, mass };
}

Particle::Particle()
{
	isActive = false;
	lifetime = 0;
	particleModel = { {}, { 0, 0, 0 }, 1.0f };
}

void Particle::Update(float deltaTime)
{
	particleModel.Update(deltaTime);
}