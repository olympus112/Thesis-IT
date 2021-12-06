#include "core.h"
#include "patch.h"

Patch::Patch(const Shape& shape) {
	this->shape = shape;
	this->mask = computeMask(shape);
}

Mask Patch::computeMask(const Shape& shape) {
	Mask mask(shape);

	return mask;
}