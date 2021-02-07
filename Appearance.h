#pragma once

#include "Resources/Structs.h"

class Appearance
{
private:
	MeshData meshData;
	ID3D11ShaderResourceView* texture;

	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 specularMaterial;

	bool transparent;
	bool billboard;

public:
	Appearance() {}
	Appearance(MeshData mesh, ID3D11ShaderResourceView* tex, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, bool trans, bool bill);
	~Appearance();

	bool GetTransparent() { return transparent; }
	bool GetBillboard() { return billboard; }
	XMFLOAT4 GetDiffuse() { return diffuseMaterial; }
	XMFLOAT4 GetAmbient() { return ambientMaterial; }
	XMFLOAT4 GetSpecular() { return specularMaterial; }
	ID3D11ShaderResourceView*& GetTexture() { return texture; }
	MeshData& GetMesh() { return meshData; }
};