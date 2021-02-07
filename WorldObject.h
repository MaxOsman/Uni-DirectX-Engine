#pragma once

#include "Resources/Structs.h"
#include "Camera.h"
#include "Transform.h"
#include "ParticleModel.h"
#include "Appearance.h"

class WorldObject
{
private:
	Transform* transform;
	ParticleModel* particleModel;
	Appearance* appearance;
	bool isStaticTerrain;

public:
	WorldObject(Transform* t, Appearance* ap, Vector3D v, float m, bool staticTerrain);
	~WorldObject();

	bool GetTransparent() { return appearance->GetTransparent(); }
	Vector3D GetPos() { return transform->GetPos(); }
	Vector3D GetRot() { return transform->GetRot(); }
	Vector3D GetScale() { return transform->GetScale(); }
	Transform* GetTransform() { return transform; }
	ParticleModel* GetParticle() { return particleModel; }
	Appearance* GetAppearance() { return appearance; }
	bool GetTerrain() { return isStaticTerrain; }

	void SetPos(Vector3D p) { transform->SetPos(p); }
	void SetRot(Vector3D r) { transform->SetRot(r); }
	void SetScale(Vector3D s) { transform->SetScale(s); }

	void CameraTranslate(XMFLOAT3 d, Camera* cam);
	void PlayerMove(XMFLOAT3 a, Camera* cam);
	void PlayerJump(float jump);

	void Update(float deltaTime);
	void Render(ConstantBuffer cb, ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* pConstantBuffer, ID3D11BlendState* transparency, float yaw);
};