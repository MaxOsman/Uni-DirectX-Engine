#pragma once

#include "Structs.h"
#include "Constants.h"
#include "Transform.h"

enum CollisionType
{
	COLLISION_SPHERE,
	COLLISION_AABB
};

enum Directions
{
	FORWARD,
	BACK,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct CollisionData
{
	bool isCollided;
	Directions direction;
	Vector3D difference;
};

class ParticleModel
{
private:
	Vector3D velocity;
	Vector3D acceleration;

	CollisionType collisionType;
	float boundSphereRadius;
	Vector3D widths;
	Vector3D corner;

	Vector3D netForce;
	Vector3D thrustForce;
	Vector3D frictionForce;
	Vector3D dragForce;
	Vector3D weightForce;
	Vector3D reactionForce;

	float mass;
	bool isLaminar;
	bool isGrounded = false;
	bool isCollided = false;
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

	bool SphereCollision(Vector3D position, float radius);
	bool AABBCollision(Vector3D corner2, Vector3D widths2);
	CollisionData SphereAABBCollision(Vector3D position1, float radius1, Vector3D corner2, Vector3D widths2);
	Directions VectorDirection(Vector3D target);

	void SetVelocityX(float x) { velocity.x = x; }
	void SetVelocityY(float y) { velocity.y = y; }
	void SetVelocityZ(float z) { velocity.z = z; }
	void SetVelocity(Vector3D v) { velocity = v; }
	Vector3D GetVelocity() { return velocity; }
	void SetAccel(Vector3D a) { acceleration = a; }
	Vector3D GetAccel() { return acceleration; }
	void SetMass(float m) { mass = m; }
	float GetMass() { return mass; }
	void SetGrounded(bool g) { isGrounded = g; }
	bool GetGrounded() { return isGrounded; }

	float GetRadius() { return boundSphereRadius; }
	void SetRadius(float r) { boundSphereRadius = r; collisionType = COLLISION_SPHERE; }
	Vector3D GetCorner() { return corner; }
	Vector3D GetWidths() { return widths; }
	CollisionType GetCollisionType() { return collisionType; }
	void SetCollided(bool c) { isCollided = c; }
	void SetAABBProperties(Vector3D w, Vector3D c) { widths = w; corner = c; collisionType = COLLISION_AABB; }

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