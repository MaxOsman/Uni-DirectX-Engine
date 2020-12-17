#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "OBJLoader.h"

using namespace DirectX;

#define PLAYEROBJECT 0

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 DiffuseLight;
	XMFLOAT3 LightVecW;
	float gTime;
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 AmbientLight;
	XMFLOAT4 SpecularMtrl;
	XMFLOAT4 SpecularLight;
	float SpecularPower;
	XMFLOAT3 EyePosW;
};

struct WorldObject
{
	MeshData meshData;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scale;
	ID3D11ShaderResourceView* texture;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 specularMaterial;
	bool trans;
	//float t;
};

struct IndexedMesh
{
	MeshData meshData;
	std::string name;
};

struct IndexedTex
{
	ID3D11ShaderResourceView* texture;
	std::string name;
};

struct IndexedMat
{
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 specularMaterial;
	std::string name;
};

enum CameraMode
{
	CAMERA_ANGLED,
	CAMERA_TOPDOWN,
	CAMERA_FIRST,
	CAMERA_THIRD
};