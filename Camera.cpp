#include "Camera.h"

Camera::Camera(UINT windowHeight, UINT windowWidth, Vector3D eye, Vector3D at, Vector3D up)
{
    _windowWidth = windowWidth;
    _windowHeight = windowHeight;
    _eye = eye;
    _at = at;
    _up = up;

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet(_eye.x, _eye.y, _eye.z, 0.0f);
    XMVECTOR At = XMVectorSet(_at.x, _at.y, _at.z, 0.0f);
    XMVECTOR Up = XMVectorSet(_up.x, _up.y, _up.z, 0.0f);
    XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

    // Initialize the projection matrix
    XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth * XM_PI / (FLOAT)_windowHeight, 0.01f, 100.0f));

    pitch = 0.0f;
    yaw = 0.0f;
    rotationSpeed = 0.005f;
    r = 3.0f;
}

void Camera::Update(CameraMode mode, HWND hWnd, Vector3D pos)
{
    _mode = mode;
    if (_mode == CAMERA_FIRST || _mode == CAMERA_THIRD)
    {
        //Get screen coords
        GetClientRect(hWnd, &rc);
        MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rc), 2);

        //Check position
        GetCursorPos(&point);
        POINT newPoint = {point.x - (0.5 * rc.right + 0.5 * rc.left), point.y - (0.5 * rc.bottom + 0.5 * rc.top)};

        //Do player rotation
        Rotate(newPoint.x, newPoint.y);

        //Reset position
        SetCursorPos(0.5 * rc.right + 0.5 * rc.left, 0.5 * rc.bottom + 0.5 * rc.top);

        _eye = _monkey;
        XMStoreFloat4x4(&_view, (_mode == CAMERA_FIRST ? GetMatrix1st() : GetMatrix3rd()));
    }
    else if (_mode == CAMERA_TOPDOWN)
    {
        ChangePos({ pos.x, pos.y + 20.0f, pos.z }, pos, _up);
    }
}

void Camera::Reshape(UINT windowWidth, UINT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
    XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth * XM_PI / (FLOAT)windowHeight, nearDepth, farDepth));
}

void Camera::ChangePos(Vector3D e, Vector3D a, Vector3D u)
{
    _eye = e;
    _at = a;
    _up = u;
    XMVECTOR Eye = XMVectorSet(e.x, e.y, e.z, 0.0f);
    XMVECTOR At = XMVectorSet(a.x, a.y, a.z, 0.0f);
    XMVECTOR Up = XMVectorSet(u.x, u.y, u.z, 0.0f);
    XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
}

void Camera::Rotate(float dx, float dy)
{
    yaw = WrapAngle(yaw + (dx * rotationSpeed));
    pitch = Clamp(pitch + (dy * rotationSpeed), 0.995f * XM_PI / 2.0f, 0.995f * -XM_PI / 2.0f);
}

XMMATRIX Camera::GetMatrix1st()
{
    const XMVECTOR lookVector = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f));

    XMFLOAT3 tempMatrix = { _eye.x, _eye.y, _eye.z };
    const XMVECTOR camPosition = XMLoadFloat3(&tempMatrix);
    const XMVECTOR camTarget = camPosition + lookVector;

    return XMMatrixLookAtLH(camPosition, camTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

XMMATRIX Camera::GetMatrix3rd()
{
    XMFLOAT3 tempMatrix;
    XMStoreFloat3(&tempMatrix, XMVector3Transform(XMVectorSet(0.0f, 0.0f, -r, 0.0f), XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)));

    _eye = { _eye.x + tempMatrix.x, _eye.y + tempMatrix.y, _eye.z + tempMatrix.z };
    tempMatrix = { _eye.x, _eye.y, _eye.z };

    const XMVECTOR pos = XMVector3Transform(XMVectorSet(0.0f, 0.0f, -r, 0.0f), XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f));
    return XMMatrixLookAtLH(pos + XMLoadFloat3(&tempMatrix), XMLoadFloat3(&tempMatrix), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

float Camera::WrapAngle(float ang)
{
    ang = fmod(ang + 180, 360);
    if (ang < 0)
        ang += 360;

    ang -= 180;
    return ang;
}

float Camera::Clamp(float x, float upper, float lower)
{
    return min(upper, max(x, lower));
}