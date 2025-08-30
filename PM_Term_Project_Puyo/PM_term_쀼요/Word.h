#pragma once
#include <string>
#include <GL/glut.h>

#include "Vector3f.h"

using namespace std;

class Word
{
public:
	Word();
	Word(string w);
	Word(string w, void* f);

	void setWord(const string& w);
	void setFont(void* f);
	void setCoor(const float& x, const float& y);
	void setColor(const Vector3f& v);

	char getInitial() const;

	float& operator[](const int i);
	float operator[](const int i) const;

	void display() const;

private:
	string word;
	void* font;
	float coor[2];
	Vector3f color;
};