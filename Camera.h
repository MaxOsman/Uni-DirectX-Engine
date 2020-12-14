#pragma once

#include "Structs.h"
#include "Math.h"

class Camera
{
private:
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

	RECT rc;
	POINT point;

	XMFLOAT3 _eye;
	XMFLOAT3 _at;
	XMFLOAT3 _up;
	XMFLOAT3 _monkey;

	float pitch = 0.0f;
	float yaw = 0.0f;
	float walkSpeed;
	float rotationSpeed = 0.005f;
	float phi;
	float theta;
	float r = 3.0f;

	UINT _windowHeight;
	UINT _windowWidth;

public:
	Camera(UINT windowHeight, UINT windowWidth, XMFLOAT3 eye, XMFLOAT3 at, XMFLOAT3 up, float speed);
	~Camera() {};

	void Update(CameraMode mode, HWND hWnd);

	XMFLOAT4X4 GetView() { return _view; }
	XMFLOAT4X4 GetProjection() { return _projection; }
	XMFLOAT3 GetEye() { return _eye; }
	float GetYaw() { return yaw; }
	float GetPitch() { return pitch; }
	
	void Reshape(UINT windowWidth, UINT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	void ChangePos(XMFLOAT3 e, XMFLOAT3 a, XMFLOAT3 u);
	void SetMonkey(XMFLOAT3 monke) { _monkey = monke; }

	//void Translate(float dx, float dy, float dz);
	void Rotate1st(float dx, float dy);
	void Rotate3rd(float dx, float dy);
	XMMATRIX GetMatrix1st();
	XMMATRIX GetMatrix3rd();
	float WrapAngle(float a);
	float Clamp(float value, float upper, float lower);
};