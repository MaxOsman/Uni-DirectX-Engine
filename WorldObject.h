#pragma once

#include "Structs.h"
#include "Camera.h"

class WorldObject
{
private:
	MeshData meshData;

	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scale;

	ID3D11ShaderResourceView* texture;

	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 specularMaterial;

	bool transparent;
	bool billboard;

	float blendFactor[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
	float _playerSpeed;

public:
	WorldObject(MeshData mesh, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, ID3D11ShaderResourceView* tex, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, bool trans, bool bill);
	~WorldObject();

	XMFLOAT3 GetPos() { return pos; }
	XMFLOAT3 GetRot() { return rot; }
	bool GetTrans() { return transparent; }

	void SetPos(XMFLOAT3 p) { pos = p; }
	void SetRot(XMFLOAT3 r) { rot = r; }
	void Translate(XMFLOAT3 d, Camera* cam);
	void SetSpeed();

	void Render(ConstantBuffer cb, ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* pConstantBuffer, ID3D11BlendState* transparency, float yaw);
};