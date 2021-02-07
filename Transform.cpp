#include "Transform.h"

Transform::Transform(Vector3D p, Vector3D r, Vector3D s)
{
    pos = p;
    rot = r;
    scale = s;
}

void Transform::Translate(Vector3D d)
{
    pos += d;
}