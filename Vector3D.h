#pragma once

#include <math.h>

class Vector3D
{
public:
	float x, y, z;

	Vector3D();
	Vector3D(float _x, float _y, float _z);
	Vector3D(const Vector3D &vec);

	Vector3D operator+(const Vector3D& vec);		//addition
	Vector3D& operator+=(const Vector3D& vec);		//assign new result to vector
	Vector3D operator-(const Vector3D& vec);		//substraction
	Vector3D& operator-=(const Vector3D& vec);		//assign new result to vector
	Vector3D operator*(float value);				//multiplication
	Vector3D& operator*=(float value);				//assign new result to vector.
	Vector3D operator/(float value);				//division
	Vector3D& operator/=(float value);				//assign new result to vector
	Vector3D& operator=(const Vector3D& vec);

	float dot_product(const Vector3D& vec);			//scalar dot_product
	Vector3D cross_product(const Vector3D& vec);	//cross_product
	Vector3D normalization();						//normalized vector

	float square();									//gives square of the vector
	float distance(const Vector3D& vec);			//distance between two vectors
	float magnitude();								//magnitude of the vector
};