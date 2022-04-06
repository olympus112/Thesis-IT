#include "core.h"
#include "patch.h"

Patch::Patch(const Shape& shape, const Vec2& sourceOffset, const Vec2& targetOffset) {
	this->shape = shape;
	this->sourceOffset = sourceOffset;
	this->targetOffset = targetOffset;

	this->mask = computeMask(shape);
}

void Patch::render(const Canvas& source, const Canvas& target, bool fill, const Color& color) const {
	shape.render(source, source.toRelativeScreenSpace(sourceOffset), fill, color);
	shape.render(target, target.toRelativeScreenSpace(targetOffset), fill, color);
}

void Patch::render(const Bounds& bounds, bool fill, const Color& color) const {
	shape.render(bounds, fill, color);
}

Mask& Patch::computeMask(const Shape& shape) {
	this->mask = Mask(shape);

	return this->mask;
}

Match Patch::computeMatch(const std::vector<cv::Mat>& sourceTextures, const std::vector<cv::Mat>& targetTextures, const std::vector<double>& distribution) {
	return Match(mask, sourceOffset, targetOffset, sourceTextures, targetTextures, distribution);
}

Vec2 Patch::dimension() const {
	return shape.dimension;
}

Bounds Patch::targetBounds() const {
	return Bounds(
		targetOffset,
		dimension()
	);
}

Bounds Patch::sourceBounds() const {
	return Bounds(
		sourceOffset,
		dimension()
	);
}