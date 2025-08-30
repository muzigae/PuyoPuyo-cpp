#include "Word.h"

Word::Word() {
	word = "";
	font = GLUT_BITMAP_TIMES_ROMAN_24;
	//font = GLUT_BITMAP_HELVETICA_18;
	coor[0] = coor[1] = 0;
	color = { 1, 1, 1 };
}

Word::Word(string w) {
	word = w;
	//font = GLUT_BITMAP_TIMES_ROMAN_24;
	font = GLUT_BITMAP_HELVETICA_18;
	//font = GLUT_BITMAP_9_BY_15;
	coor[0] = coor[1] = 0;
	color = { 1, 1, 1 };
}

Word::Word(string w, void* f) {
	word = w;
	font = f;
	coor[0] = coor[1] = 0;
	color = { 1, 1, 1 };
}

void Word::setWord(const string& w) {
	word = w;
}
void Word::setFont(void* f) {
	font = f;
}
void Word::setCoor(const float& x, const float& y) {
	coor[0] = x;
	coor[1] = y;
}
void Word::setColor(const Vector3f& v) {
	color = v;
}

char Word::getInitial() const { return word[0]; }

float& Word::operator[](const int i) { return coor[i]; }
float Word::operator[](const int i) const { return coor[i]; }

void Word::display() const {
	glColor3f(color[0], color[1], color[2]);
	glRasterPos2f(coor[0], coor[1]);
	
	for (int i = 0; i < word.size(); i++)
		glutBitmapCharacter(font, word[i]);
}