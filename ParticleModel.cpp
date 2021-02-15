#include "ParticleModel.h"

ParticleModel::ParticleModel(Transform* t, Vector3D v, float m)
{
    transform = t;
    velocity = v;
    mass = m;

    isLaminar = false;
    weightForce = { 0.0f, -G * mass, 0.0f };
}

ParticleModel::ParticleModel()
{
    
}

ParticleModel::~ParticleModel()
{
    transform = nullptr;
}

void ParticleModel::Update(const float deltaTime)
{
    UpdateGround();
    UpdateFriction();
    UpdateDrag();
    UpdateGravity();

    UpdateNetForce();
    UpdateAccel();

    MoveConstVelocity(deltaTime);
    MoveConstAcceleration(deltaTime);
}

void ParticleModel::MoveConstVelocity(const float deltaTime)
{
    transform->SetPos(transform->GetPos() + velocity*deltaTime);
}

void ParticleModel::MoveConstAcceleration(const float deltaTime)
{
    //Update position
    transform->SetPos(transform->GetPos() + velocity*deltaTime + acceleration*0.5f*deltaTime*deltaTime);

    //Update velocity
    velocity = velocity + acceleration*deltaTime;
}

void ParticleModel::UpdateNetForce()
{
    netForce = thrustForce + frictionForce + dragForce + weightForce + reactionForce;
}

void ParticleModel::UpdateAccel()
{
    acceleration = netForce / mass;
}

void ParticleModel::UpdateFriction()
{
    if (isGrounded && netForce.magnitude() != 0.0f)
    {
        float FrictionMag = 2.0f * mass * G;
        if (thrustForce.magnitude() <= FrictionMag && velocity.magnitude() <= 0.1f)
        {
            //Static friction
            frictionForce = thrustForce * -1.0f;
            velocity = { 0.0f, 0.0f, 0.0f };
            acceleration = { 0.0f, 0.0f, 0.0f };
        }
        else
        {
            FrictionMag = 1.0f * mass * G;
            Vector3D frictionDir = velocity * -1.0f / velocity.magnitude();
            frictionForce = frictionDir * FrictionMag;
        }
    }
    else
        frictionForce = { 0.0f, 0.0f, 0.0f };
}

void ParticleModel::UpdateDrag()
{
    if (isLaminar)
    {
        dragForce = velocity * -1.0f;
    }
    else
    {  
        if (velocity.magnitude() != 0)
        {
            float dragMag = -velocity.magnitude() * velocity.magnitude();
            dragForce = (velocity / velocity.magnitude()) * dragMag;
        }
        else
            dragForce = { 0.0f, 0.0f, 0.0f };
    }
}

void ParticleModel::UpdateGravity()
{
    if (isGrounded)
    {
        reactionForce = { 0.0f, G * mass, 0.0f };
    }
    else
    {
        reactionForce = { 0.0f, 0.0f, 0.0f };
    }      
}

void ParticleModel::UpdateGround()
{
    if (transform->GetPos().y <= 0.9f)
    {
        if (!isGrounded)
        {
            isGrounded = true;
        }
        velocity.y = 0.0f;
        transform->SetPos({ transform->GetPos().x, 0.9f, transform->GetPos().z });
    }
    else
        isGrounded = false;
}