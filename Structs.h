#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include "resource.h"
#include "OBJLoader.h"
#include "Vector3D.h"

using namespace DirectX;

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