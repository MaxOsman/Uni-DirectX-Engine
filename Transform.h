#pragma once

#include "Structs.h"
#include "Quaternion.h"

class Transform
{
private:
	Vector3D pos;
	Quaternion orientation;
	Vector3D scale;
	XMFLOAT4X4 worldMatrix;

	bool billboard;

public:
	Transform();
	Transform(Vector3D p, Vector3D r, Vector3D s, bool bill);

	void Update(float yaw, float pitch);
	void Render(ConstantBuffer* cb);

	Vector3D QuatToEuler(Quaternion q);
	Quaternion EulerToQuat(Vector3D e);
	void Translate(Vector3D d);

	Vector3D GetPos() { return pos; }
	Vector3D GetRot() { return QuatToEuler(orientation); }
	Quaternion GetOri() { return orientation; }
	Vector3D GetScale() { return scale; }
	XMFLOAT4X4 GetWorld() { return worldMatrix; }
	bool GetBillboard() { return billboard; }

	void SetPos(Vector3D p) { pos = p; }
	void SetPosX(float x) { pos.x = x; }
	void SetPosY(float y) { pos.y = y; }
	void SetPosZ(float z) { pos.z = z; }
	void SetRot(Vector3D r) { orientation = EulerToQuat(r); }
	void SetOri(Quaternion q) { orientation = q; }
	void SetScale(Vector3D s) { scale = s; }
};