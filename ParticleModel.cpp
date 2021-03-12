#include "ParticleModel.h"

ParticleModel::ParticleModel(Transform* t, Vector3D v, float m)
{
    transform = t;
    velocity = v;
    mass = m;

    isLaminar = false;
    weightForce = { 0.0f, -G * mass, 0.0f };
}

ParticleModel::ParticleModel() {}

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

bool ParticleModel::SphereCollision(Vector3D position, float radius)
{
    return (position.distance(transform->GetPos()) < (radius + boundSphereRadius));
}

bool ParticleModel::AABBCollision(Vector3D corner2, Vector3D widths2)
{
    // developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
    /*if((corner.x + transform->GetPos().x <= corner2.x + widths2.x && corner.x + transform->GetPos().x + widths.x >= corner2.x) &&
        (corner.y + transform->GetPos().y <= corner2.y + widths2.y && corner.y + transform->GetPos().y + widths.y >= corner2.y) &&
        (corner.z + transform->GetPos().z <= corner2.z + widths2.z && corner.z + transform->GetPos().z + widths.z >= corner2.z))
    return { true, FORWARD, distance };
    
    return { false, FORWARD, { 0.0f, 0.0f, 0.0f } };*/

    return  (corner.x + transform->GetPos().x <= corner2.x + widths2.x && corner.x + transform->GetPos().x + widths.x >= corner2.x) &&
            (corner.y + transform->GetPos().y <= corner2.y + widths2.y && corner.y + transform->GetPos().y + widths.y >= corner2.y) &&
            (corner.z + transform->GetPos().z <= corner2.z + widths2.z && corner.z + transform->GetPos().z + widths.z >= corner2.z);
}

CollisionData ParticleModel::SphereAABBCollision(Vector3D position1, float radius1, Vector3D corner2, Vector3D widths2)
{
    // developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
    float x = max(corner2.x, min(position1.x, corner2.x + widths2.x));
    float y = max(corner2.y, min(position1.y, corner2.y + widths2.y));
    float z = max(corner2.z, min(position1.z, corner2.z + widths2.z));

    //float distance = sqrt((x - position1.x) * (x - position1.x) + (y - position1.y) * (y - position1.y) + (z - position1.z) * (z - position1.z));
    Vector3D distance = { x - position1.x , y - position1.y , z - position1.z };
    if (distance.magnitude() < radius1)
        return { true, VectorDirection(distance), distance };

    return { false, FORWARD, { 0.0f, 0.0f, 0.0f } };
}

Directions ParticleModel::VectorDirection(Vector3D target)
{
    // learnopengl.com/In-Practice/2D-Game/Collisions/Collision-resolution
    Vector3D directions[6] = {  Vector3D(1.0f, 0.0f, 0.0f),
                                Vector3D(-1.0f, 0.0f, 0.0f),
                                Vector3D(0.0f, 1.0f, 0.0f),
                                Vector3D(0.0f, -1.0f, 0.0f),
                                Vector3D(0.0f, 0.0f, 1.0f),
                                Vector3D(0.0f, 0.0f, -1.0f) };

    float max = 0.0f;
    short bestMatch = -1;
    for (short x = 0; x < 6; ++x)
    {
        float dotProduct = target.normalization().dot_product(directions[x]);
        if (dotProduct > max)
        {
            max = dotProduct;
            bestMatch = x;
        }
    }

    return (Directions)bestMatch;
}