#include "WorldObject.h"

WorldObject::WorldObject(Transform* t, Appearance* ap, Vector3D v, float m, bool staticTerrain)
{
    transform = t;
    appearance = ap;
    isStaticTerrain = staticTerrain;
    if (staticTerrain)
        particleModel = nullptr;  
    else
    {
        particleModel = new ParticleModel(transform, v, m);

        inertiaTensor._11 = particleModel->GetMass() * (width * width * transform->GetScale().y) * (width * width * transform->GetScale().z) / 12.0f;
        inertiaTensor._12 = 0;
        inertiaTensor._13 = 0;
        inertiaTensor._21 = 0;
        inertiaTensor._22 = particleModel->GetMass() * (width * width * transform->GetScale().x) * (width * width * transform->GetScale().z) / 12.0f;
        inertiaTensor._23 = 0;
        inertiaTensor._31 = 0;
        inertiaTensor._32 = 0;
        inertiaTensor._33 = particleModel->GetMass() * (width * width * transform->GetScale().x) * (width * width * transform->GetScale().y) / 12.0f;

        /*if(appearance->GetName() != "Cube")
            particleModel->SetRadius(width * transform->GetScale().y / 2);
        else
            particleModel->SetAABBProperties(   { width * transform->GetScale().x, width * transform->GetScale().y, width * transform->GetScale().z },
                                                {   -width * transform->GetScale().x / 2,
                                                    -width * transform->GetScale().y / 2,
                                                    -width * transform->GetScale().z / 2 });*/

        particleModel->SetRadius(width * transform->GetScale().x / 2.0f);
    }
}

WorldObject::~WorldObject()
{
    transform = nullptr;
    appearance = nullptr;
    particleModel = nullptr;
}

void WorldObject::Render(ConstantBuffer cb, ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* pConstantBuffer, ID3D11BlendState* transparency)
{
    appearance->Render(&cb, pImmediateContext, transparency);
    transform->Render(&cb);

    pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
    pImmediateContext->DrawIndexed(appearance->GetMesh().IndexCount, 0, 0);
}

void WorldObject::Update(float deltaTime, float yaw, float pitch)
{
    particleModel->Update(deltaTime);
    transform->Update(yaw, pitch);

    CalcAngularAccel();
    UpdateRotationalSpeed(deltaTime);
    UpdateRotationalOrientation(deltaTime);
}

void WorldObject::UpdateRotationalSpeed(float deltaTime)
{
   angVelocity = angVelocity * pow(angDamping, deltaTime) + angAccel * deltaTime;
}

void WorldObject::UpdateRotationalOrientation(float deltaTime)
{
    Quaternion qMine = transform->GetOri();

    qMine.addScaledVector(angVelocity, deltaTime);
    qMine.normalise();

    transform->SetOri(qMine);

    XMMATRIX mOut;
    CalculateTransformMatrixRowMajor(mOut, transform->GetPos(), qMine);

    XMStoreFloat4x4(&transform->GetWorld(), mOut);

    XMVECTOR outTrans, outScale, outQ;
    XMMatrixDecompose(&outScale, &outQ, &outTrans, XMLoadFloat4x4(&transform->GetWorld()));
    XMFLOAT4 rot;
    XMStoreFloat4(&rot, outQ);

    /*debug->DebugString("rotY:" + std::to_string(transform->GetRot().y) + "  ");
    debug->DebugString("accY:" + std::to_string(angAccel.y) + "  ");
    debug->DebugString("velY:" + std::to_string(angVelocity.y) + "  ");*/
}

void WorldObject::CalcAngularAccel()
{
    XMMATRIX tempM = XMLoadFloat3x3(&inertiaTensor);
    tempM = XMMatrixInverse(nullptr, tempM);

    XMFLOAT3 tempT = { torque.x, torque.y, torque.z };
    XMVECTOR torq = XMLoadFloat3(&tempT);
    torq = XMVector3Transform(torq, tempM);

    XMFLOAT3 accel;
    XMStoreFloat3(&accel, torq);
    angAccel = { accel.x, accel.y, accel.z };
}

void WorldObject::CameraTranslate(XMFLOAT3 d, float pitch, float yaw)
{
    XMStoreFloat3(&d, XMVector3Transform(XMLoadFloat3(&d), XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)));
    //XMStoreFloat3(&d, XMVector3Transform(XMLoadFloat3(&d), XMMatrixRotationRollPitchYaw(0.0f, cam->GetYaw(), 0.0f)));
    transform->SetPos({ GetTransform()->GetPos().x + d.x, GetTransform()->GetPos().y + d.y, GetTransform()->GetPos().z + d.z });
}

void WorldObject::PlayerMove(XMFLOAT3 a, float yaw)
{
    XMStoreFloat3(&a, XMVector3Transform(XMLoadFloat3(&a), XMMatrixRotationRollPitchYaw(0.0f, yaw, 0.0f)));
    Vector3D tempVec = { a.x, a.y, a.z };
    particleModel->SetThrust(particleModel->GetThrust()+tempVec);
}

void WorldObject::PlayerJump(float jump)
{
    if (particleModel->GetGrounded())
    {
        Vector3D tempVec = { 0.0f, jump, 0.0f };
        particleModel->SetThrust(particleModel->GetThrust() + tempVec);
    }
}