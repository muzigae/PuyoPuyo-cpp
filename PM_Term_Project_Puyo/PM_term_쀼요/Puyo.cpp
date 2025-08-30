#include "Puyo.h"

Vector3f moveX(1, 0, 0);
Vector3f moveY(0, 1, 0);

Puyo::Puyo() {
	rad[0] = 0; slice[0] = 0; stack[0] = 0;
	color = 0;
}

Puyo::Puyo(float r, int sl, int st) {
	rad[0] = r; slice[0] = sl; stack[0] = st;
	color = 0;
}

const float* Puyo::getRadius() const { return rad; }
const int* Puyo::getSlice() const { return slice; }
const int* Puyo::getStack() const { return stack; }
const int& Puyo::getColor() const { return color; }

void Puyo::setColor(const int& c) {
	color = c;
}

void Puyo::draw() const {
	glPushMatrix();

	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_EMISSION, &getEmission()[0]);
	glMaterialfv(GL_FRONT, GL_AMBIENT, &getAmbient()[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &getDiffuse()[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &getSpecular()[0]);
	glMaterialfv(GL_FRONT, GL_SHININESS, getShininess());

	glTranslatef(-fieldX + getCenter()[0]*2*radius, fieldY_bottom + getCenter()[1]*2*radius, getCenter()[2]); // 중심 위치 보내기

	glutSolidSphere(getRadius()[0], getSlice()[0], getStack()[0]);

	glPopMatrix();
}

Puyo_pair::Puyo_pair() {
	puyo1 = puyo2 = Puyo();
	rot = 0;
}

void Puyo_pair::setPuyoPair(const Puyo& p1, const Puyo& p2) {
	puyo1 = p1; puyo2 = p2;
}
void Puyo_pair::setFirstPuyo(const Puyo& p){
	puyo1 = p;
}
void Puyo_pair::setSecondPuyo(const Puyo& p){
	puyo2 = p;
}

Puyo& Puyo_pair::getFirstPuyo() { return puyo1; }
Puyo& Puyo_pair::getSecondPuyo() { return puyo2; }

void Puyo_pair::left() {
	puyo1.setCenter(puyo1.getCenter() - moveX);
	puyo2.setCenter(puyo2.getCenter() - moveX);
}
void Puyo_pair::right() {
	puyo1.setCenter(puyo1.getCenter() + moveX);
	puyo2.setCenter(puyo2.getCenter() + moveX);
}
void Puyo_pair::down() {
	puyo1.setCenter(puyo1.getCenter() - moveY);
	puyo2.setCenter(puyo2.getCenter() - moveY);
}
void Puyo_pair::rotate() { // puyo1이 반시계 회전
	rot += 1;
	if (rot == 4) rot = 0;

	switch(rot) {
	case 0: // 왼쪽
		puyo1.setCenter(puyo2.getCenter() - moveX);
		break;
	case 1: // 위쪽
		puyo1.setCenter(puyo2.getCenter() + moveY);
		break;
	case 2: // 오른쪽
		puyo1.setCenter(puyo2.getCenter() + moveX);
		break;
	case 3: // 아래쪽
		puyo1.setCenter(puyo2.getCenter() - moveY);
	}
}

void Puyo_pair::initRot() {
	rot = 0;
}

const int& Puyo_pair::getRotate() const { return rot; }

void Puyo_pair::draw() const {
	puyo1.draw(); puyo2.draw();
}