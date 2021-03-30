#pragma once

#include "Structs.h"

#define BLENDFACTOR { 0.75f, 0.75f, 0.75f, 1.0f }

class Appearance
{
private:
	MeshData meshData;
	ID3D11ShaderResourceView* texture;
	std::string name;
	bool transparent;

	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 specularMaterial;

public:
	Appearance() {}
	Appearance(MeshData mesh, ID3D11ShaderResourceView* tex, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, bool trans, std::string n);
	~Appearance();

	void Render(ConstantBuffer* cb, ID3D11DeviceContext* pImmediateContext, ID3D11BlendState* transparency);

	bool GetTransparent() { return transparent; }
	XMFLOAT4 GetDiffuse() { return diffuseMaterial; }
	XMFLOAT4 GetAmbient() { return ambientMaterial; }
	XMFLOAT4 GetSpecular() { return specularMaterial; }
	ID3D11ShaderResourceView*& GetTexture() { return texture; }
	MeshData& GetMesh() { return meshData; }
	std::string GetName() { return name; }
};