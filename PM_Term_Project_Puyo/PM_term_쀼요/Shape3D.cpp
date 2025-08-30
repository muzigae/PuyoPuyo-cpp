#include "Shape3D.h"

Shape3D::Shape3D() {
	setCenter(0, 0, 0);
	setVelocity(0, 0, 0);
}

void Shape3D::setCenter(float x, float y, float z) {
	center.coor(x, y, z);
}
void Shape3D::setCenter(const Vector3f& c) {
	center.coor(c[0], c[1], c[2]);
}
Vector3f Shape3D::getCenter() const { return center; }

void Shape3D::setVelocity(float x, float y, float z) {
	velocity.coor(x, y, z);
}
void Shape3D::setVelocity(const Vector3f& v) {
	velocity.coor(v[0], v[1], v[2]);
}
Vector3f Shape3D::getVelocity() const { return velocity; }

void Shape3D::move() {
	setCenter(getCenter() + getVelocity());
}

float& Shape3D::operator[](const int i) { return center[i]; }
float Shape3D::operator[](const int i) const { return center[i]; }