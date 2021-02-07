#include "Appearance.h"

Appearance::Appearance(MeshData mesh, ID3D11ShaderResourceView* tex, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, bool trans, bool bill)
{
	meshData = mesh;
	texture = tex;
	diffuseMaterial = diffuse;
	ambientMaterial = ambient;
	specularMaterial = specular;
	transparent = trans;
	billboard = bill;
}

Appearance::~Appearance()
{
	texture = nullptr;
}