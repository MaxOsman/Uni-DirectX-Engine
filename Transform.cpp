#include "Transform.h"

Transform::Transform(Vector3D p, Vector3D r, Vector3D s, bool bill)
{
    pos = p;
    orientation = EulerToQuat(r);
    scale = s;
    billboard = bill;
}

Transform::Transform()
{
    pos = { 0, 0, 0 };
    orientation = EulerToQuat({ 0, 0, 0 });
    scale = { 1, 1, 1 };
    billboard = false;
}

void Transform::Translate(Vector3D d)
{
    pos += d;
}

Vector3D Transform::QuatToEuler(Quaternion q)
{
    // math.stackexchange.com/questions/2975109/how-to-convert-euler-angles-to-quaternions-and-get-the-same-euler-angles-back-fr

    float a = 2.0f * (q.r * q.i + q.j * q.k);
    float b = 1.0f - 2.0f * (q.i * q.i + q.j * q.j);
    float x = atan2(a, b);

    float c = 2.0f * (q.r * q.j - q.k * q.i);
    if (c > 1.0f)
        c = 1.0f;
    else if (c < -1.0f)
        c = -1.0f;
    float y = asin(c);

    float d = 2.0f * (q.r * q.k + q.i * q.j);
    float e = 1.0f - 2.0f * (q.j * q.j + q.k * q.k);
    float z = atan2(d, e);

    return { x, y, z };
}

Quaternion Transform::EulerToQuat(Vector3D e)
{
    // math.stackexchange.com/questions/2975109/how-to-convert-euler-angles-to-quaternions-and-get-the-same-euler-angles-back-fr

    float x = sin(e.x / 2) * cos(e.y / 2) * cos(e.z / 2) - cos(e.x / 2) * sin(e.y / 2) * sin(e.z / 2);
    float y = cos(e.x / 2) * sin(e.y / 2) * cos(e.z / 2) + sin(e.x / 2) * cos(e.y / 2) * sin(e.z / 2);
    float z = cos(e.x / 2) * cos(e.y / 2) * sin(e.z / 2) - sin(e.x / 2) * sin(e.y / 2) * cos(e.z / 2);
    float w = cos(e.x / 2) * cos(e.y / 2) * cos(e.z / 2) + sin(e.x / 2) * sin(e.y / 2) * sin(e.z / 2);

    return { w, x, y, z };
}

void Transform::Update(float yaw, float pitch)
{
    Quaternion tempOri;

    if (billboard)
    {
        Vector3D rot = GetRot();
        rot.x -= pitch;
        rot.y += yaw + XM_PI;
        tempOri = EulerToQuat(rot);
    }
    else
        tempOri = orientation;

    XMFLOAT3 tempScale = { scale.x, scale.y, scale.z };

    XMMATRIX rotMatrix;
    CalculateTransformMatrixRowMajor(rotMatrix, pos, tempOri);
    XMMATRIX scaleMatrix = XMMatrixScaling(tempScale.x, tempScale.y, tempScale.z);

    XMMATRIX tempCombo = XMMatrixMultiply(scaleMatrix, rotMatrix);
    XMStoreFloat4x4(&worldMatrix, tempCombo);
}

void Transform::Render(ConstantBuffer* cb)
{ 
    cb->mWorld = XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix));
}