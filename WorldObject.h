#pragma once

#include "Structs.h"
#include "Camera.h"
#include "Transform.h"
#include "ParticleModel.h"
#include "Appearance.h"
#include "Quaternion.h"
#include "Debug.h"

class WorldObject
{
private:
	Transform* transform;
	ParticleModel* particleModel;
	Appearance* appearance;
	bool isStaticTerrain;
	std::string name;

	Debug* debug = new Debug();

	float width = 2.0f;			//Temporary, only true for cube objects!
	XMFLOAT3X3 inertiaTensor;
	float angDamping = 1.0f;
	Vector3D torque = {0,0,0};
	Vector3D angAccel = {0,0,0};
	Vector3D angVelocity = {0,0,0};
	XMFLOAT4X4 worldMatrix;

public:
	WorldObject(Transform* t, Appearance* ap, Vector3D v, float m, bool staticTerrain, std::string n);
	~WorldObject();

	bool GetTransparent() { return appearance->GetTransparent(); }
	Vector3D GetPos() { return transform->GetPos(); }
	Vector3D GetRot() { return transform->GetRot(); }
	Vector3D GetScale() { return transform->GetScale(); }
	Transform* GetTransform() { return transform; }
	ParticleModel* GetParticle() { return particleModel; }
	Appearance* GetAppearance() { return appearance; }
	bool GetTerrain() { return isStaticTerrain; }
	std::string GetName() { return name; }

	void SetPos(Vector3D p) { transform->SetPos(p); }
	void SetRot(Vector3D r) { transform->SetRot(r); }
	void SetScale(Vector3D s) { transform->SetScale(s); }
	void SetAngAccel(Vector3D a) { angAccel = a; }

	void CalcTorque(Vector3D force) { torque = transform->GetPos().cross_product(force); }
	void CalcAngularAccel();
	void UpdateRotationalSpeed(float deltaTime);
	void UpdateRotationalOrientation(float deltaTime);

	void CameraTranslate(XMFLOAT3 d, Camera* cam);
	void PlayerMove(XMFLOAT3 a, Camera* cam);
	void PlayerJump(float jump);

	void Update(float deltaTime);
	void UpdateWorldMatrix();
	void Render(ConstantBuffer cb, ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* pConstantBuffer, ID3D11BlendState* transparency, float yaw);
};