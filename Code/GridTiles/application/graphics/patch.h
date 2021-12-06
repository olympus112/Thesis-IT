#pragma once

#include "mask.h"
#include "shape.h"

class Patch {
public:
	Shape shape;
	Mask mask;

	Patch(const Shape& shape);

	static Mask computeMask(const Shape& shape);
};

