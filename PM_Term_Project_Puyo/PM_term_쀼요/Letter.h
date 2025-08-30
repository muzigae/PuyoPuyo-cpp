#pragma once
#include <cmath>
#include "Shape3D.h"

#define PI 3.141592

class Letter : public Shape3D
{
public:
	virtual void draw() const;
};