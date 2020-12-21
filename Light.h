#pragma once

#include "Structs.h"

class Light
{
private:
	XMFLOAT3 lightDirection;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 ambientLight;
	XMFLOAT4 specularLight;
	float specularPower;
	XMFLOAT3 eyePosWorld;

public:
	Light();
	~Light();

	void Update();
	void Draw(ConstantBuffer *cb);

	void SetEye(XMFLOAT3 eye) { eyePosWorld = eye; }
	void AddDirection(XMFLOAT3 dir) { lightDirection = {lightDirection.x + dir.x, lightDirection.y + dir.y , lightDirection.z + dir.z }; }
};