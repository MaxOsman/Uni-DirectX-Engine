#pragma once

#include "Transform.h"
#include "ParticleModel.h"
#include "Appearance.h"
#include "Debug.h"

class WorldObject
{
private:
	Transform* transform;
	ParticleModel* particleModel;
	Appearance* appearance;
	bool isStaticTerrain;

	Debug* debug = new Debug();

	float width = 2.0f;
	XMFLOAT3X3 inertiaTensor;
	float angDamping = 1.0f;
	Vector3D torque = { 0, 0, 0 };
	Vector3D angAccel = { 0, 0, 0 };
	Vector3D angVelocity = { 0, 0, 0 };

public:
	WorldObject(Transform* t, Appearance* ap, float m, bool staticTerrain);
	~WorldObject();

	Transform* GetTransform() { return transform; }
	ParticleModel* GetParticle() { return particleModel; }
	Appearance* GetAppearance() { return appearance; }
	bool GetTerrain() { return isStaticTerrain; }

	void SetAngAccel(Vector3D a) { angAccel = a; }
	void CalcTorque(Vector3D force) { torque = transform->GetPos().cross_product(force); }
	void CalcAngularAccel();
	void UpdateRotationalSpeed(float deltaTime);
	void UpdateRotationalOrientation(float deltaTime);

	void CameraTranslate(XMFLOAT3 d, float pitch, float yaw);
	void PlayerMove(XMFLOAT3 a, float yaw);
	void PlayerJump(float jump);

	void Update(float deltaTime, float yaw, float pitch);
	void Render(ConstantBuffer cb, ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* pConstantBuffer, ID3D11BlendState* transparency);
};