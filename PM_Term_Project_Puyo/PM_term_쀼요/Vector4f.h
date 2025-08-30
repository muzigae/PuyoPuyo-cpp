#pragma once

class Vector4f
{
public:
	Vector4f();
	Vector4f(float x, float y, float z, float w);

	void coor(float x, float y, float z, float w);

	float& operator[](const int i);
	float operator[](const int i) const;

private:
	float pos[4];
};

// operator
Vector4f operator+(const Vector4f& v1, const Vector4f& v2);
Vector4f operator-(const Vector4f& v1, const Vector4f& v2);
Vector4f operator/(const Vector4f& v, const float s); // scalar division
Vector4f operator*(const Vector4f& v, const float s); // scalar multiplication