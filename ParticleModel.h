#pragma once

#include "Structs.h"
#include "Constants.h"
#include "Transform.h"

class ParticleModel
{
private:
	Vector3D velocity;
	Vector3D acceleration;

	Vector3D netForce;
	Vector3D thrustForce;
	Vector3D frictionForce;
	Vector3D dragForce;
	Vector3D weightForce;
	Vector3D reactionForce;

	float mass;
	bool isLaminar;
	bool isGrounded = false;
	Transform* transform;

public:
	ParticleModel(Transform* t, Vector3D v, float m);
	ParticleModel();
	~ParticleModel();

	void Update(const float deltaTime);

	void MoveConstVelocity(const float deltaTime);
	void MoveConstAcceleration(const float deltaTime);
	void UpdateNetForce();
	void UpdateAccel();
	void UpdateFriction();
	void UpdateDrag();
	void UpdateGravity();
	void UpdateGround();

	void SetVelocity(Vector3D v) { velocity = v; }
	Vector3D GetVelocity() { return velocity; }
	void SetAccel(Vector3D a) { acceleration = a; }
	Vector3D GetAccel() { return acceleration; }
	void SetMass(float m) { mass = m; }
	float GetMass() { return mass; }
	bool GetGrounded() { return isGrounded; }

	void SetNetForce(Vector3D f) { netForce = f; }
	Vector3D GetNetForce() { return netForce; }
	void SetThrust(Vector3D f) { thrustForce = f; }
	Vector3D GetThrust() { return thrustForce; }
	void SetFriction(Vector3D f) { frictionForce = f; }
	Vector3D GetFriction() { return frictionForce; }
	void SetLaminarFlow(bool lam) { isLaminar = lam; }
	bool GetLaminarFlow() { return isLaminar; }
	void SetTransform(Transform t) { transform = &t; }
	Transform* GetTransform() { return transform; }
};