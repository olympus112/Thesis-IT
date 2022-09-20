#pragma once
#include <opencv2/imgproc.hpp>

#include "patch.h"

struct MondriaanPatch {
public:
	// Patch offset in the source texture, this is the topleft point of the circumscribing axis aligned bounding box
	Vec2f sourceOffset;
	// Patch offset in the target texture, this is the topleft point of the circumscribing axis aligned bounding box
	Vec2f targetOffset;
	// Dimension of the patch in millimeters
	Vec2f dimension_mm;
	// Dimension of the patch in pixels
	Vec2i dimension_px;
	// Rotation index
	int rotationIndex;


	// Last computed match
	double sortingScore;

public:
	MondriaanPatch();
	MondriaanPatch(const Vec2f& sourceOffset, const Vec2f& targetOffset, const Vec2f& dimension);

	void render(const Canvas& source, const Canvas& target, bool intersected, bool selected, bool showConnections, const Color& sourceColor, const Color& targetColor, bool invert = false) const;

	Bounds sourceBounds() const;
	Bounds sourceBoundsRelative() const;
	Bounds targetBounds() const;
	Bounds targetBoundsRelative() const;
	Bounds sourceRotatedBounds(bool invert = false) const;

	Vec2 sourceDimension() const;
	Vec2 targetDimension() const;
	Vec2 sourceRotatedDimension(bool invert = false) const;
	Vec2 sourceRotatedDimension2f(bool invert = false) const;

	Bounds sourceUV() const;
	Bounds sourceRotatedUV() const;
	Bounds targetUV() const;

	void computeMatch();
	void computeBestMatch(cv::TemplateMatchModes metric = cv::TM_SQDIFF);

	void removeFromGlobalMask() const;
	void addToGlobalMask() const;

	std::vector<Vec2> sourcePoints() const;
	std::vector<Vec2> sourceRotatedPoints(bool invert = false) const;
	std::vector<Vec2> sourceRotatedPointsRelative2f(bool invert = false) const;
	std::vector<Vec2> targetPoints() const;
};