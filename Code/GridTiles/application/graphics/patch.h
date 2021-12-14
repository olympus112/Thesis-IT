#pragma once

#include "mask.h"
#include "match.h"
#include "shape.h"

class Patch {
public:
	// Normalized shape
	Shape shape;
	// Normalized mask
	Mask mask;
	// Patch offset in the source texture
	Vec2 sourceOffset;
	// Patch offset in the target texture
	Vec2 targetOffset;

	Patch(const Shape& shape, const Vec2& sourceOffset, const Vec2& targetOffset);

	void render(const Canvas& source, const Canvas& target, bool fill = false, const Color& color = Colors::WHITE) const;
	void render(const Bounds& bounds, bool fill = false, const Color& color = Colors::WHITE) const;

	Mask& computeMask(const Shape& shape);
	Match computeMatch(const std::vector<cv::Mat>& sourceTextures, const std::vector<cv::Mat>& targetTextures, const std::vector<double>& distribution);

	Vec2 dimension() const;
};

