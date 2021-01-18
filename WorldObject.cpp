#include "WorldObject.h"

WorldObject::WorldObject(MeshData mesh, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, ID3D11ShaderResourceView* tex, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, bool trans, bool bill)
{
	meshData = mesh;
	pos = p;
	rot = r;
	scale = s;
	texture = tex;
	diffuseMaterial = diffuse;
	ambientMaterial = ambient;
	specularMaterial = specular;
	transparent = trans;
    billboard = bill;

    _playerSpeed = 10.0f;
}

WorldObject::~WorldObject()
{
    
}

void WorldObject::Render(ConstantBuffer cb, ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* pConstantBuffer, ID3D11BlendState* transparency, float yaw)
{
    // Set the blend state for transparent objects
    if (transparent)
        pImmediateContext->OMSetBlendState(transparency, blendFactor, 0xffffffff);
    else
        pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

    //Billboarding
    XMFLOAT3 tempRot = rot;
    if (billboard)
        tempRot.y += yaw + XM_PI;

    cb.DiffuseMtrl = diffuseMaterial;
    cb.AmbientMtrl = ambientMaterial;
    cb.SpecularMtrl = specularMaterial;

    pImmediateContext->PSSetShaderResources(0, 1, &texture);

    pImmediateContext->IASetVertexBuffers(0, 1, &meshData.VertexBuffer, &meshData.VBStride, &meshData.VBOffset);
    pImmediateContext->IASetIndexBuffer(meshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    XMMATRIX posMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);
    XMMATRIX rotMatrix = XMMatrixRotationX(tempRot.x) * XMMatrixRotationY(tempRot.y) * XMMatrixRotationZ(tempRot.z);
    XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
    XMMATRIX temp = XMMatrixMultiply(XMMatrixMultiply(rotMatrix, scaleMatrix), posMatrix);
    XMFLOAT4X4 tempWorld;
    XMStoreFloat4x4(&tempWorld, temp);
    cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&tempWorld));

    pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
    pImmediateContext->DrawIndexed(meshData.IndexCount, 0, 0);
}

void WorldObject::Translate(XMFLOAT3 d, Camera* cam)
{
    XMStoreFloat3(&d, XMVector3Transform(XMLoadFloat3(&d), XMMatrixRotationRollPitchYaw(cam->GetPitch(), cam->GetYaw(), 0.0f) * XMMatrixScaling(_playerSpeed, _playerSpeed, _playerSpeed)));
    pos = { pos.x + d.x, pos.y + d.y, pos.z + d.z };

    if (cam->GetMode() == CAMERA_TOPDOWN)
    {
        XMFLOAT3 camEye = cam->GetEye();
        XMFLOAT3 camAt = cam->GetAt();
        cam->ChangePos({ camEye.x + d.x, camEye.y + d.y, camEye.z + d.z }, pos, cam->GetUp());
    }
}

void WorldObject::SetSpeed()
{
    if (_playerSpeed == 10.0f)
        _playerSpeed = 20.0f;
    else
        _playerSpeed = 10.0f;
}