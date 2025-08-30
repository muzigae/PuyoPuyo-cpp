#include "Beam.h"

Beam::Beam() {
	setAmbient(1, 1, 1, 1);
	setDiffuse(1, 1, 1, 1);
	setSpecular(1, 1, 1, 1);
	position.coor(0, 0, 0, 1);
}

Beam::Beam(float x, float y, float z) { // 0 init
	setAmbient(1, 1, 1, 1);
	setDiffuse(1, 1, 1, 1);
	setSpecular(1, 1, 1, 1);
	position.coor(x, y, z, 1);
}

void Beam::setAmbient(float r, float g, float b, float a) {
	ambient.coor(r, g, b, a);
}

void Beam::setDiffuse(float r, float g, float b, float a) {
	diffuse.coor(r, g, b, a);
}

void Beam::setSpecular(float r, float g, float b, float a) {
	specular.coor(r, g, b, a);
}

Vector4f Beam::getPosition() const { return position; }
Vector4f Beam::getAmbient() const { return ambient; }
Vector4f Beam::getDiffuse() const { return diffuse; }
Vector4f Beam::getSpecular() const { return specular; }

void Beam::draw() const { // LIGHT ID는 0으로 고정
	glPushMatrix();

	glLightfv(GL_LIGHT0, GL_AMBIENT, &getAmbient()[0]);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, &getDiffuse()[0]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, &getSpecular()[0]);
	glLightfv(GL_LIGHT0, GL_POSITION, &getPosition()[0]);

	glPopMatrix();
}