#include "core.h"
#include "patch.h"

Patch::Patch(const Shape& shape, const Vec2& sourceOffset, const Vec2& targetOffset) {
	this->shape = shape;
	this->sourceOffset = sourceOffset;
	this->targetOffset = targetOffset;

	this->mask = computeMask(shape);
}

Mask& Patch::computeMask(const Shape& shape) {
	this->mask = Mask(shape);

	return this->mask;
}

Match Patch::computeMatch(const std::vector<cv::Mat>& sourceTextures, const std::vector<cv::Mat>& targetTextures, const std::vector<double>& distribution) {
	return Match(mask, sourceOffset, targetOffset, sourceTextures, targetTextures, distribution);
}

