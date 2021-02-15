#include "Transform.h"

Transform::Transform(Vector3D p, Vector3D r, Vector3D s)
{
    pos = p;
    rot = r;
    scale = s;
}

Transform::Transform()
{
    pos = { 0, 0, 0 };
    rot = { 0, 0, 0 };
    scale = { 0, 0, 0 };
}

void Transform::Translate(Vector3D d)
{
    pos += d;
}