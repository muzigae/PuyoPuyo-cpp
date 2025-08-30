#include "Vector3f.h"

Vector3f::Vector3f() { coor(0, 0, 0); }
Vector3f::Vector3f(float x, float y, float z) { coor(x, y, z); }

void Vector3f::coor(float x, float y, float z) {
	pos[0] = x; pos[1] = y; pos[2] = z;
}

// operator
float& Vector3f::operator[](const int i) { return pos[i]; }
float Vector3f::operator[](const int i) const { return pos[i]; }

Vector3f operator+(const Vector3f& v1, const Vector3f& v2) { return Vector3f(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]); }
Vector3f operator-(const Vector3f& v1, const Vector3f& v2) { return Vector3f(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]); }
Vector3f operator*(const Vector3f& v, const float s) { return Vector3f(v[0] * s, v[1] * s, v[2] * s); }
Vector3f operator/(const Vector3f& v, const float s) { return Vector3f(v[0] / s, v[1] / s, v[2] / s); }