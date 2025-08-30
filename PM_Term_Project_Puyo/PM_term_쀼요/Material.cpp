#include "Material.h"

Material::Material() {
	setEmission(0.1, 0.1, 0.1, 1.0);
	setAmbient(0.5, 0.5, 0.5, 1.0);
	setDiffuse(1.0, 1.0, 1.0, 1.0);
	setSpecular(1.0, 1.0, 1.0, 1.0);
	setShininess(50.0);
}

void Material::setEmission(float r, float g, float b, float a) {
	emission.coor(r, g, b, a);
}

void Material::setAmbient(float r, float g, float b, float a) {
	ambient.coor(r, g, b, a);
}
void Material::setAmbient(const Vector4f& v) { // colorset 사용을 위해 정의
	ambient.coor(v[0], v[1], v[2], v[3]);
}

void Material::setDiffuse(float r, float g, float b, float a) {
	diffuse.coor(r, g, b, a);
}

void Material::setSpecular(float r, float g, float b, float a) {
	specular.coor(r, g, b, a);
}

void Material::setShininess(float sh) {
	shininess[0] = sh;
}

Vector4f Material::getEmission() const { return emission; }
Vector4f Material::getAmbient() const { return ambient; }
Vector4f Material::getDiffuse() const { return diffuse; }
Vector4f Material::getSpecular() const { return specular; }
const float* Material::getShininess() const { return shininess; }