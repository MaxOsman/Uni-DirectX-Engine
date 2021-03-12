#include "WorldObject.h"

WorldObject::WorldObject(Transform* t, Appearance* ap, Vector3D v, float m, bool staticTerrain, std::string n)
{
    transform = t;
    appearance = ap;
    isStaticTerrain = staticTerrain;
    name = n;
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

        /*if(name != "Cube")
            particleModel->SetRadius(width * transform->GetScale().y / 2);
        else*/
            particleModel->SetAABBProperties(   { width * transform->GetScale().x, width * transform->GetScale().y, width * transform->GetScale().z },
                                                {   -width * transform->GetScale().x / 2,
                                                    -width * transform->GetScale().y / 2,
                                                    -width * transform->GetScale().z / 2 });
    }
}

WorldObject::~WorldObject()
{
    transform = nullptr;
    appearance = nullptr;
    particleModel = nullptr;
}

void WorldObject::Render(ConstantBuffer cb, ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* pConstantBuffer, ID3D11BlendState* transparency, float yaw)
{
    // Set the blend state for transparent objects
    if (appearance->GetTransparent())
    {
        float blend[4] = BLENDFACTOR;
        pImmediateContext->OMSetBlendState(transparency, blend, 0xffffffff);
    }
    else
        pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

    //Billboarding
    /*XMFLOAT3 tempPos = { transform->GetPos().x, transform->GetPos().y, transform->GetPos().z };
    XMFLOAT3 tempRot = { transform->GetRot().x, transform->GetRot().y, transform->GetRot().z };
    XMFLOAT3 tempScale = { transform->GetScale().x, transform->GetScale().y, transform->GetScale().z };*/
    /*if (appearance->GetBillboard())
        tempRot.y += yaw + XM_PI;*/

    cb.DiffuseMtrl = appearance->GetDiffuse();
    cb.AmbientMtrl = appearance->GetAmbient();
    cb.SpecularMtrl = appearance->GetSpecular();

    if (name == "GridMesh")
    {
        //pImmediateContext->PSSetShaderResources(0, 1, &appearance->GetTexture());
    }
    else
        pImmediateContext->PSSetShaderResources(0, 1, &appearance->GetTexture());

    pImmediateContext->IASetVertexBuffers(0, 1, &appearance->GetMesh().VertexBuffer, &appearance->GetMesh().VBStride, &appearance->GetMesh().VBOffset);
    pImmediateContext->IASetIndexBuffer(appearance->GetMesh().IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix));

    pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
    pImmediateContext->DrawIndexed(appearance->GetMesh().IndexCount, 0, 0);
}

void WorldObject::Update(float deltaTime)
{
    particleModel->Update(deltaTime);

    UpdateWorldMatrix();
    CalcAngularAccel();
    UpdateRotationalSpeed(deltaTime);
    UpdateRotationalOrientation(deltaTime);
}

void WorldObject::UpdateWorldMatrix()
{
    XMFLOAT3 tempPos = { transform->GetPos().x, transform->GetPos().y, transform->GetPos().z };
    XMFLOAT3 tempRot = { transform->GetRot().x, transform->GetRot().y, transform->GetRot().z };
    XMFLOAT3 tempScale = { transform->GetScale().x, transform->GetScale().y, transform->GetScale().z };

    XMMATRIX posMatrix = XMMatrixTranslation(tempPos.x, tempPos.y, tempPos.z);
    XMMATRIX rotMatrix = XMMatrixRotationX(tempRot.x) * XMMatrixRotationY(tempRot.y) * XMMatrixRotationZ(tempRot.z);
    XMMATRIX scaleMatrix = XMMatrixScaling(tempScale.x, tempScale.y, tempScale.z);

    XMMATRIX tempCombo = XMMatrixMultiply(XMMatrixMultiply(rotMatrix, scaleMatrix), posMatrix);
    XMStoreFloat4x4(&worldMatrix, tempCombo);
}

void WorldObject::UpdateRotationalSpeed(float deltaTime)
{
   angVelocity = angVelocity * pow(angDamping, deltaTime) + angAccel * deltaTime;
}

void WorldObject::UpdateRotationalOrientation(float deltaTime)
{
    XMVECTOR q = XMQuaternionRotationMatrix(XMLoadFloat4x4(&worldMatrix));

    XMFLOAT4 qf4;
    XMStoreFloat4(&qf4, q);

    Quaternion qMine;
    qMine.i = qf4.x;
    qMine.j = qf4.y;
    qMine.k = qf4.z;
    qMine.r = qf4.w;

    //qMine.rotateByVector(Vector3D(0, 0, 0));

    qMine.addScaledVector(angVelocity, deltaTime);
    qMine.normalise();

    XMMATRIX mOut;
    CalculateTransformMatrixRowMajor(mOut, transform->GetPos(), qMine);

    XMStoreFloat4x4(&worldMatrix, mOut);

    XMVECTOR outTrans, outScale, outQ;
    XMMatrixDecompose(&outScale, &outQ, &outTrans, XMLoadFloat4x4(&worldMatrix));
    XMFLOAT4 rot;
    XMStoreFloat4(&rot, outQ);
    transform->SetRot({ rot.x, rot.y, rot.z });

    /*XMMATRIX rot = XMMatrixRotationX(transform->GetRot().x);
    rot *= XMMatrixRotationY(transform->GetRot().y);
    rot *= XMMatrixRotationZ(transform->GetRot().z);

    XMVECTOR quatVector;
    quatVector = XMQuaternionRotationMatrix(rot);

    XMFLOAT4 tempQ;
    XMStoreFloat4(&tempQ, quatVector);

    Quaternion myQuat(tempQ.w, tempQ.x, tempQ.y, tempQ.z);
    myQuat.addScaledVector(angVelocity, deltaTime);
    myQuat.normalise();

    quat->i = transform->GetRot().x;
    quat->j = transform->GetRot().y;
    quat->k = transform->GetRot().z;

    quat->addScaledVector(angVelocity, deltaTime);
    quat->normalise();

    CalculateTransformMatrixRowMajor(XMLoadFloat4x4(&worldMatrix), {0,0,0}, myQuat);

    Vector3D temp = { myQuat.i, myQuat.j, myQuat.k };
    transform->SetRot( temp );*/

    debug->DebugString("rotY:" + std::to_string(transform->GetRot().y) + "  ");
    debug->DebugString("accY:" + std::to_string(angAccel.y) + "  ");
    debug->DebugString("velY:" + std::to_string(angVelocity.y) + "  ");
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

void WorldObject::CameraTranslate(XMFLOAT3 d, Camera* cam)
{
    XMStoreFloat3(&d, XMVector3Transform(XMLoadFloat3(&d), XMMatrixRotationRollPitchYaw(cam->GetPitch(), cam->GetYaw(), 0.0f)));
    //XMStoreFloat3(&d, XMVector3Transform(XMLoadFloat3(&d), XMMatrixRotationRollPitchYaw(0.0f, cam->GetYaw(), 0.0f)));
    SetPos({ GetPos().x + d.x, GetPos().y + d.y, GetPos().z + d.z });
}

void WorldObject::PlayerMove(XMFLOAT3 a, Camera* cam)
{
    XMStoreFloat3(&a, XMVector3Transform(XMLoadFloat3(&a), XMMatrixRotationRollPitchYaw(0.0f, cam->GetYaw(), 0.0f)));
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