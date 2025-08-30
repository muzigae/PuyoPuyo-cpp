#pragma once
#include "Material.h"

#define WIDTH 336
#define HEIGHT 660

#define viewX WIDTH/2
#define viewY HEIGHT/2

class Shape3D
{
public:
	Shape3D();

	void setCenter(float x, float y, float z);
	void setCenter(const Vector3f& c);
	Vector3f getCenter() const;

	void setVelocity(float x, float y, float z);
	void setVelocity(const Vector3f& v);
	Vector3f getVelocity() const;

	float& operator[](const int i);
	float operator[](const int i) const;

	void move();
	virtual void draw() const = 0;

protected:
	Vector3f center;
	Vector3f velocity;
};