#pragma once

#include "Structs.h"
#include "Constants.h"
#include "Math.h"

class Camera
{
private:
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;
	CameraMode _mode;

	RECT rc;
	POINT point;

	Vector3D _eye;
	Vector3D _at;
	Vector3D _up;
	Vector3D _monkey;

	float pitch;
	float yaw;
	float rotationSpeed;
	float r;

	UINT _windowHeight;
	UINT _windowWidth;

public:
	Camera(UINT windowHeight, UINT windowWidth, Vector3D eye, Vector3D at, Vector3D up);

	void Update(CameraMode mode, HWND hWnd, Vector3D pos);

	XMFLOAT4X4 GetView() { return _view; }
	XMFLOAT4X4 GetProjection() { return _projection; }
	Vector3D GetEye() { return _eye; }
	Vector3D GetAt() { return _at; }
	Vector3D GetUp() { return _up; }
	CameraMode GetMode() { return _mode; }
	float GetYaw() { return yaw; }
	float GetPitch() { return pitch; }
	
	void Reshape(UINT windowWidth, UINT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	void ChangePos(Vector3D e, Vector3D a, Vector3D u);
	void SetMonkey(Vector3D monke) { _monkey = monke; }
	void AddR(float R) { if(r > MINIMUMZOOM || R > 0) r += R; }
	void Rotate(float dx, float dy);

	XMMATRIX GetMatrix1st();
	XMMATRIX GetMatrix3rd();
	float WrapAngle(float a);
	float Clamp(float value, float upper, float lower);
};