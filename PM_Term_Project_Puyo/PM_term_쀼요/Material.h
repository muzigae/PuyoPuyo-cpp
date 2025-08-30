#pragma once
#include "Beam.h"

class Material
{
public:
	Material();

	void setEmission(float r, float g, float b, float a);
	Vector4f getEmission() const;

	void setAmbient(float r, float g, float b, float a);
	void setAmbient(const Vector4f& v); // colorset 사용을 위해 정의
	Vector4f getAmbient() const;

	void setDiffuse(float r, float g, float b, float a);
	Vector4f getDiffuse() const;

	void setSpecular(float r, float g, float b, float a);
	Vector4f getSpecular() const;

	void setShininess(float sh);
	const float* getShininess() const;

protected:
	Vector4f emission;
	Vector4f ambient;
	Vector4f diffuse;
	Vector4f specular;
	float shininess[1];
};