#pragma once

#include "Resources/Structs.h"

class Transform
{
private:
	Vector3D pos;
	Vector3D rot;
	Vector3D scale;

public:
	Transform() {};
	Transform(Vector3D p, Vector3D r, Vector3D s);

	void Translate(Vector3D d);

	Vector3D GetPos() { return pos; }
	Vector3D GetRot() { return rot; }
	Vector3D GetScale() { return scale; }

	void SetPos(Vector3D p) { pos = p; }
	void SetRot(Vector3D r) { rot = r; }
	void SetScale(Vector3D s) { scale = s; }
};