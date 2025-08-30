#include "Letter.h"

void Letter::draw() const {
}

Letter pu1, yo1, pu2, yo2;

// 도형으로 만든 글자
void draw뿌(const Letter& pu, const float& w) {
	glColor3f(1, 1, 1);
	glLineWidth(w);
	glBegin(GL_LINES); {
		glVertex3f(pu[0] - 0.5, pu[1], 1); glVertex3f(pu[0] + 0.5, pu[1] + 0.2, 1); // 중심 ㅡ
		glVertex3f(pu[0] - 0.5, pu[1], 1); glVertex3f(pu[0] - 0.5, pu[1] + 1, 1); // 왼쪽 ㅣ
		glVertex3f(pu[0] + 0.5, pu[1] + 0.2, 1); glVertex3f(pu[0] + 0.5, pu[1] + 0.8, 1); // 오른쪽 ㅣ
		glVertex3f(pu[0] - 0.4, pu[1] + 0.25, 1); glVertex3f(pu[0] + 0.4, pu[1] + 0.4, 1); // 가운데 ㅡ
		glVertex3f(pu[0] , pu[1] + 0.1, 1); glVertex3f(pu[0], pu[1] + 0.6, 0); // 가운데 ㅣ
		glVertex3f(pu[0] - 0.2, pu[1] - 0.2, 1); glVertex3f(pu[0] + 0.4, pu[1] - 0.1, 1); // 아래 ㅡ
		glVertex3f(pu[0] + 0.1, pu[1] - 0.15, 1); glVertex3f(pu[0] + 0.1, pu[1] - 0.4, 1); // 아래 ㅣ
	} glEnd();
}

void draw요(const Letter& yo, const float& r, const float& w1, const float& w2) {
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 50; i++) {
		glVertex3f(yo[0] + r * cos(2 * PI / 50 * i), yo[1] + r * sin(2 * PI / 50 * i), 1);
		glVertex3f(yo[0] + r * w1 * cos(2 * PI / 50 * i), yo[1] + r * w1 * sin(2 * PI / 50 * i), 1);
	} glEnd();

	glLineWidth(w2);
	glBegin(GL_LINES); {
		glVertex3f(yo[0] - r * 2 / 3, yo[1] - r - r * 1 / 2, 1); glVertex3f(yo[0] - r * 1.7 / 3, yo[1] - r - r * 5 / 4, 1); // 왼쪽 ㅣ
		glVertex3f(yo[0] + r * 3 / 5, yo[1] - r - r * 1 / 5, 1); glVertex3f(yo[0] + r * 2.5 / 5, yo[1] - r - r * 8 / 5, 1); // 오른쪽 ㅣ
		glVertex3f(yo[0] - r * 3 / 2, yo[1] - r - r * 11 / 5, 1); glVertex3f(yo[0] + r * 4.5 / 2, yo[1] - r - r * 6 / 5, 1); // 아래 ㅡ
	} glEnd();
}