#include "ParticleModel.h"

ParticleModel::ParticleModel(Transform* t, float m)
{
    transform = t;
    mass = m;

    isLaminar = false;
    weightForce = { 0.0f, -G * mass, 0.0f };
}

ParticleModel::ParticleModel()
{
    transform = {};
    mass = 1.0f;

    isLaminar = false;
    weightForce = { 0.0f, -G, 0.0f };
    collisionType = COLLISION_NONE;
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
    netForce =  thrustForce
                + frictionForce 
                + dragForce 
                + weightForce 
                + reactionForce
                ;
}

void ParticleModel::UpdateAccel()
{
    acceleration = netForce / mass;
}

void ParticleModel::UpdateFriction()
{
    if ((isGrounded || isOnObject) && netForce.magnitude() != 0.0f)
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
            if (velocity.magnitude() != 0.0f)
            {
                FrictionMag = 1.0f * mass * G;
                Vector3D frictionDir = velocity * -1.0f / velocity.magnitude();
                frictionForce = frictionDir * FrictionMag;
            }
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
    reactionForce = (isGrounded || isOnObject ? Vector3D( 0.0f, G * mass, 0.0f ) : Vector3D( 0.0f, 0.0f, 0.0f ));
}

void ParticleModel::UpdateGround()
{
    if (transform->GetPos().y - widths.y / 2 <= 0.0f)
    {
        if (!isGrounded)
        {
            isGrounded = true;
        }
        velocity.y = 0.0f;
        transform->SetPos({ transform->GetPos().x, widths.y / 2, transform->GetPos().z });
    }
    else
        isGrounded = false;
}

CollisionData ParticleModel::SphereCollision(Vector3D position, float radius)
{
    if (position.distance(transform->GetPos()) < (radius + boundSphereRadius))
    {
        Vector3D distance = position - transform->GetPos();
        if(distance.magnitude() != 0.0f)
            return { true, VectorDirection(distance), distance };
    }
       
    return { false, FORWARD, { 0.0f, 0.0f, 0.0f } };
}

CollisionData ParticleModel::AABBCollision(Vector3D corner1, Vector3D widths1, Vector3D corner2, Vector3D widths2)
{
    // gamedev.stackexchange.com/questions/129446/how-can-i-calculate-the-penetration-depth-between-two-colliding-3d-aabbs

    // Random large number
    float* mtvDistance = new float(9000000.0f);
    Vector3D* mtvAxis = new Vector3D();

    if ((!TestAxis({ 1.0f, 0.0f, 0.0f }, corner1.x, widths1.x, corner2.x, widths2.x, mtvAxis, mtvDistance)) ||
        (!TestAxis({ 0.0f, 1.0f, 0.0f }, corner1.y, widths1.y, corner2.y, widths2.y, mtvAxis, mtvDistance)) ||
        (!TestAxis({ 0.0f, 0.0f, 1.0f }, corner1.z, widths1.z, corner2.z, widths2.z, mtvAxis, mtvDistance)))
        return { false, FORWARD, { 0.0f, 0.0f, 0.0f } };

    float penetration = sqrt(*mtvDistance) * 1.001f;
    Vector3D minimumTranslation = (mtvAxis->magnitude() != 0.0f ? mtvAxis->normalization() : Vector3D(0.0f, 0.0f, 0.0f));

    return { true, VectorDirection(minimumTranslation * penetration), minimumTranslation * penetration };
}

bool ParticleModel::TestAxis(Vector3D axis, float corner1, float width1, float corner2, float width2, Vector3D* mtvAxis, float* mtvDistance)
{
    // gamedev.stackexchange.com/questions/129446/how-can-i-calculate-the-penetration-depth-between-two-colliding-3d-aabbs

    float axisLengthSquared = axis.dot_product(axis);

    if (axisLengthSquared < 1.0e-8f)
        return true;

    float leftOverlap = corner2 + width2 - corner1;
    float rightOverlap = corner1 + width1 - corner2;

    if (leftOverlap <= 0.0f || rightOverlap <= 0.0f)
        return false;

    float overlap = (leftOverlap < rightOverlap ? leftOverlap : -rightOverlap);

    Vector3D sep = axis * (overlap / axisLengthSquared);

    float sepLengthSquared = sep.dot_product(sep);
    if (sepLengthSquared < *mtvDistance)
    {
        *mtvDistance = sepLengthSquared;
        *mtvAxis = sep;
    }

    return true;
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
    {
        return { true, VectorDirection(distance), distance };
    }

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