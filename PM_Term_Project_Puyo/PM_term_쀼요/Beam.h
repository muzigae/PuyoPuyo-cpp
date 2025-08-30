#pragma once
#include "Vector3f.h"
#include "Vector4f.h"
#include <GL/glut.h>

class Beam
{
public:
	Beam();
	Beam(float x, float y, float z);

	void setAmbient(float r, float g, float b, float a);
	void setDiffuse(float r, float g, float b, float a);
	void setSpecular(float r, float g, float b, float a);

	Vector4f getPosition() const; // setPosition은 constructor로 해결
	Vector4f getAmbient() const;
	Vector4f getDiffuse() const;
	Vector4f getSpecular() const;

	void draw() const;

private:
	Vector4f position;
	Vector4f ambient;
	Vector4f diffuse;
	Vector4f specular;
};