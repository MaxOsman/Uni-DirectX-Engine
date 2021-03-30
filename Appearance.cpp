#include "Appearance.h"

Appearance::Appearance(MeshData mesh, ID3D11ShaderResourceView* tex, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, bool trans, std::string n)
{
	meshData = mesh;
	texture = tex;
	diffuseMaterial = diffuse;
	ambientMaterial = ambient;
	specularMaterial = specular;
	transparent = trans;
	name = n;
}

Appearance::~Appearance()
{
	texture = nullptr;
}

void Appearance::Render(ConstantBuffer* cb, ID3D11DeviceContext* pImmediateContext, ID3D11BlendState* transparency)
{
    // Set the blend state for transparent objects
    if (transparent)
    {
        float blend[4] = BLENDFACTOR;
        pImmediateContext->OMSetBlendState(transparency, blend, 0xffffffff);
    }
    else
        pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

    cb->DiffuseMtrl = diffuseMaterial;
    cb->AmbientMtrl = ambientMaterial;
    cb->SpecularMtrl = specularMaterial;

    if (name != "GridMesh")
        pImmediateContext->PSSetShaderResources(0, 1, &texture);

    pImmediateContext->IASetVertexBuffers(0, 1, &meshData.VertexBuffer, &meshData.VBStride, &meshData.VBOffset);
    pImmediateContext->IASetIndexBuffer(meshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
}