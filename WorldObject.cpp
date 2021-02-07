#include "WorldObject.h"

WorldObject::WorldObject(Transform* t, Appearance* ap, Vector3D v, float m, bool staticTerrain)
{
    transform = t;
    appearance = ap;
    isStaticTerrain = staticTerrain;
    if (staticTerrain)
        particleModel = nullptr;  
    else
        particleModel = new ParticleModel(transform, v, m);
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
    XMFLOAT3 tempPos = { transform->GetPos().x, transform->GetPos().y, transform->GetPos().z };
    XMFLOAT3 tempRot = { transform->GetRot().x, transform->GetRot().y, transform->GetRot().z };
    XMFLOAT3 tempScale = { transform->GetScale().x, transform->GetScale().y, transform->GetScale().z };
    if (appearance->GetBillboard())
        tempRot.y += yaw + XM_PI;

    cb.DiffuseMtrl = appearance->GetDiffuse();
    cb.AmbientMtrl = appearance->GetAmbient();
    cb.SpecularMtrl = appearance->GetSpecular();

    pImmediateContext->PSSetShaderResources(0, 1, &appearance->GetTexture());

    pImmediateContext->IASetVertexBuffers(0, 1, &appearance->GetMesh().VertexBuffer, &appearance->GetMesh().VBStride, &appearance->GetMesh().VBOffset);
    pImmediateContext->IASetIndexBuffer(appearance->GetMesh().IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    XMMATRIX posMatrix = XMMatrixTranslation(tempPos.x, tempPos.y, tempPos.z);
    XMMATRIX rotMatrix = XMMatrixRotationX(tempRot.x) * XMMatrixRotationY(tempRot.y) * XMMatrixRotationZ(tempRot.z);
    XMMATRIX scaleMatrix = XMMatrixScaling(tempScale.x, tempScale.y, tempScale.z);
    XMMATRIX temp = XMMatrixMultiply(XMMatrixMultiply(rotMatrix, scaleMatrix), posMatrix);
    XMFLOAT4X4 tempWorld;
    XMStoreFloat4x4(&tempWorld, temp);
    cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&tempWorld));

    pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
    pImmediateContext->DrawIndexed(appearance->GetMesh().IndexCount, 0, 0);
}

void WorldObject::Update(float deltaTime)
{
    particleModel->Update(deltaTime);
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