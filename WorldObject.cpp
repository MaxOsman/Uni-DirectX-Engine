#include "WorldObject.h"

WorldObject::WorldObject(MeshData mesh, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, ID3D11ShaderResourceView* tex, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, bool trans)
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

    _playerSpeed = 10.0f;
}

WorldObject::~WorldObject()
{
    
}

void WorldObject::Render(ConstantBuffer cb, ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* pConstantBuffer, ID3D11BlendState* transparency)
{
    // Set the blend state for transparent objects
    if (transparent)
        pImmediateContext->OMSetBlendState(transparency, blendFactor, 0xffffffff);
    else
        pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

    cb.DiffuseMtrl = diffuseMaterial;
    cb.AmbientMtrl = ambientMaterial;
    cb.SpecularMtrl = specularMaterial;

    pImmediateContext->PSSetShaderResources(0, 1, &texture);

    pImmediateContext->IASetVertexBuffers(0, 1, &meshData.VertexBuffer, &meshData.VBStride, &meshData.VBOffset);
    pImmediateContext->IASetIndexBuffer(meshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    XMMATRIX posMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);
    XMMATRIX rotMatrix = XMMatrixRotationX(rot.x) * XMMatrixRotationY(rot.y) * XMMatrixRotationZ(rot.z);
    XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
    XMMATRIX temp = XMMatrixMultiply(XMMatrixMultiply(rotMatrix, scaleMatrix), posMatrix);
    XMFLOAT4X4 tempWorld;
    XMStoreFloat4x4(&tempWorld, temp);
    cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&tempWorld));

    pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
    pImmediateContext->DrawIndexed(meshData.IndexCount, 0, 0);
}

void WorldObject::PlayerTranslate(float dx, float dy, float dz, Camera* cam)
{
    XMFLOAT3 tempMatrix = { dx, dy, dz };
    XMStoreFloat3(&tempMatrix, XMVector3Transform(XMLoadFloat3(&tempMatrix), XMMatrixRotationRollPitchYaw(cam->GetPitch(), cam->GetYaw(), 0.0f) * XMMatrixScaling(_playerSpeed, _playerSpeed, _playerSpeed)));
    SetPos({ GetPos().x + tempMatrix.x, GetPos().y + tempMatrix.y, GetPos().z + tempMatrix.z });
}

void WorldObject::SetSpeed()
{
    if (_playerSpeed == 10.0f)
        _playerSpeed = 20.0f;
    else
        _playerSpeed = 10.0f;
}