#pragma once
#include "patch.h"

struct MondriaanPatch {
public:
	// Patch offset in the source texture
	Vec2i sourceOffset;
	// Patch offset in the target texture
	Vec2i targetOffset;
	// Dimension of the patch
	Vec2 dimension_mm;

	// Source rotation
	double sourceRotation;
	// Local rotated mask
	cv::Mat mask;

public:
	MondriaanPatch(const Vec2& sourceOffset, const Vec2& targetOffset, const Vec2& dimension, double sourceRotation);

	void render(const Canvas& source, const Canvas& target, bool intersected, bool selected, bool showConnections, const Color& color) const;

	Bounds sourceBounds() const;
	Bounds targetBounds() const;

	Bounds sourceUV() const;
	Bounds targetUV() const;
};


//void mutate(int mutation, double step);
//MondriaanPatch getMutation(int mutation, double step) const;
//
//static int mutations();
//static Shape computeShape(const Vec2& dimension);