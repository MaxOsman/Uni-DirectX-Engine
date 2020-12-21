#include "Light.h"

Light::Light()
{
    lightDirection = XMFLOAT3(0.0f, 1.0f, 0.0f);
    diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    specularPower = 2.0f;
    eyePosWorld = XMFLOAT3(0.0f, 20.0f, 15.0f);
}

Light::~Light()
{

}

void Light::Draw(ConstantBuffer *cb)
{
    cb->LightVecW = lightDirection;

    cb->DiffuseLight = diffuseLight;
    cb->AmbientLight = ambientLight;
    cb->SpecularLight = specularLight;

    cb->SpecularPower = specularPower;
    cb->EyePosW = eyePosWorld;
}