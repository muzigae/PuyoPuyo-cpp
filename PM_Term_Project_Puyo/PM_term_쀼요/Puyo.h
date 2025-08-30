#pragma once
#include "Shape3D.h"

#define radius 21.f // ¹ÝÁö¸§ °íÁ¤

#define fieldX (viewX - 3*radius) // »Ñ¿ä°¡ ½×ÀÏ °ø°£ ÁÂÇ¥
#define fieldY_top (viewY - 3*radius)
#define fieldY_bottom (fieldY_top - 22*radius)

class Puyo : public Shape3D, public Material
{
public:
	Puyo();
	Puyo(float r, int sl, int st);

	const float* getRadius() const;
	const int* getSlice() const;
	const int* getStack() const;
	const int& getColor() const;

	void setColor(const int& c);

	virtual void draw() const;

private:
	float rad[1];
	int slice[1];
	int stack[1];

	int color;
};

class Puyo_pair
{
public:
	Puyo_pair();
	
	void setPuyoPair(const Puyo& p1, const Puyo& p2);
	void setFirstPuyo(const Puyo& p);
	void setSecondPuyo(const Puyo& p);

	Puyo& getFirstPuyo();
	Puyo& getSecondPuyo();

	void left();
	void right();
	void down();

	void rotate();
	void initRot();
	const int& getRotate() const;

	void draw() const;

private:
	Puyo puyo1, puyo2;
	int rot;
};