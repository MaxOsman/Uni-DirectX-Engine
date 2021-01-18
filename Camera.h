#pragma once

#include "Structs.h"
#include "Math.h"

class Camera
{
private:
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;
	CameraMode _mode;

	RECT rc;
	POINT point;

	XMFLOAT3 _eye;
	XMFLOAT3 _at;
	XMFLOAT3 _up;
	XMFLOAT3 _monkey;

	float pitch;
	float yaw;
	float rotationSpeed;
	float r;

	UINT _windowHeight;
	UINT _windowWidth;

public:
	Camera(UINT windowHeight, UINT windowWidth, XMFLOAT3 eye, XMFLOAT3 at, XMFLOAT3 up);

	void Update(CameraMode mode, HWND hWnd);

	XMFLOAT4X4 GetView() { return _view; }
	XMFLOAT4X4 GetProjection() { return _projection; }
	XMFLOAT3 GetEye() { return _eye; }
	XMFLOAT3 GetAt() { return _at; }
	XMFLOAT3 GetUp() { return _up; }
	CameraMode GetMode() { return _mode; }
	float GetYaw() { return yaw; }
	float GetPitch() { return pitch; }
	
	void Reshape(UINT windowWidth, UINT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	void ChangePos(XMFLOAT3 e, XMFLOAT3 a, XMFLOAT3 u);
	void SetMonkey(XMFLOAT3 monke) { _monkey = monke; }
	void AddR(float R) { if(r > -R*3) r += R; }
	void Rotate(float dx, float dy);

	XMMATRIX GetMatrix1st();
	XMMATRIX GetMatrix3rd();
	float WrapAngle(float a);
	float Clamp(float value, float upper, float lower);
};