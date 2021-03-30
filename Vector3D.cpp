#include "Vector3D.h"
#include <cassert>

Vector3D::Vector3D()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3D::Vector3D(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

Vector3D::Vector3D(const Vector3D& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

Vector3D Vector3D::operator+(const Vector3D &vec)
{
	return Vector3D(x + vec.x, y + vec.y, z + vec.z);
}

Vector3D& Vector3D::operator+=(const Vector3D &vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;

	return *this;
}

Vector3D Vector3D::operator-(const Vector3D& vec)
{
	return Vector3D(x - vec.x, y - vec.y, z - vec.z);
}

Vector3D& Vector3D::operator-=(const Vector3D& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;

	return *this;
}

Vector3D Vector3D::operator*(const Vector3D& vec)
{
	return Vector3D(x * vec.x, y * vec.y, z * vec.z);
}

Vector3D Vector3D::operator*(float value)
{
	return Vector3D(x * value, y * value, z * value);
}

Vector3D& Vector3D::operator*=(float value)
{
	x *= value;
	y *= value;
	z *= value;

	return *this;
}

Vector3D Vector3D::operator/(float value)
{
	assert(value != 0);
	return Vector3D(x / value, y / value, z / value);
}

Vector3D& Vector3D::operator/=(float value)
{
	assert(value != 0);

	x /= value;
	y /= value;
	z /= value;

	return *this;
}

Vector3D& Vector3D::operator=(const Vector3D& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;

	return *this;
}

float Vector3D::dot_product(const Vector3D& vec)
{
	return (x * vec.x) + (y * vec.y) + (z * vec.z);
}

Vector3D Vector3D::cross_product(const Vector3D& vec)
{
	float ni = (y * vec.z) - (z * vec.y);
	float nj = (z * vec.x) - (x * vec.z);
	float nk = (x * vec.y) - (y * vec.x);

	return Vector3D(ni, nj, nk);
}

float Vector3D::magnitude()
{
	return sqrt(square());
}

float Vector3D::square()
{
	return x*x + y*y + z*z;
}

Vector3D Vector3D::normalization()
{
	float value = magnitude();
	return Vector3D(x / value, y / value, z / value);
}

float Vector3D::distance(const Vector3D &vec)
{
	float dx = x - vec.x;
	float dy = y - vec.y;
	float dz = z - vec.z;

	return(sqrt(dx*dx + dy*dy + dz*dz));
}