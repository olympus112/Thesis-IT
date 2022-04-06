#pragma once
#include <opencv2/imgproc.hpp>

#include "patch.h"

struct MondriaanPatch {
public:
	typedef int Mutation;

	enum Mutation_ {
		Mutation_TargetMoveHorizontal = 1,
		Mutation_TargetMoveVertical = 2,
		Mutation_ResizeLeft = 3,
		Mutation_ResizeRight = 4,
		Mutation_ResizeUp = 5,
		Mutation_ResizeDown = 6,
		Mutation_Rotate = 7,
	};

	constexpr static int mutations = 7;
	constexpr static const char* mutationNames[mutations] = {
		"TargetMoveHorizontal",
		"TargetMoveVertical",
		"ResizeLeft",
		"ResizeRight",
		"ResizeUp",
		"ResizeDown",
		"Rotate"
	};

public:
	// Patch offset in the source texture, this is the topleft point of the circumscribing axis aligned bounding box
	Vec2f sourceOffset;
	// Patch offset in the target texture, this is the topleft point of the circumscribing axis aligned bounding box
	Vec2f targetOffset;
	// Dimension of the patch in millimeters
	Vec2f dimension_mm;
	// Source rotation in degrees
	float sourceRotation;

	//! To be recomputed
	// Transformation matrix
	cv::Mat transformationMatrix;
	// Inverse transformation matrix
	cv::Mat inverseTransformationMatrix;
	// Local rotated mask
	cv::Mat mask;
	// Last computed match
	double match;

public:
	MondriaanPatch();
	MondriaanPatch(const Vec2f& sourceOffset, const Vec2f& targetOffset, const Vec2f& dimension, float sourceRotation);

	void render(const Canvas& source, const Canvas& target, bool intersected, bool selected, bool showConnections, const Color& sourceColor, const Color& targetColor) const;

	Bounds sourceBounds() const;
	Bounds sourceBoundsRelative() const;
	Bounds sourceRotatedBounds() const;
	Bounds sourceRotatedBoundsRelative() const;
	Bounds sourceRotatedBoundsRelative2f() const;
	Bounds targetBounds() const;
	Bounds targetBoundsRelative() const;

	Vec2 sourceDimension() const;
	Vec2 sourceRotatedDimension() const;
	Vec2 sourceRotatedDimension2f() const;
	Vec2 targetDimension() const;

	Bounds sourceUV() const;
	Bounds sourceRotatedUV() const;
	Bounds targetUV() const;

	void computeMatch();
	void computeBestMatch(cv::TemplateMatchModes metric = cv::TM_SQDIFF);
	void computeMask();
	void computeTransformationMatrix();

	void removeFromGlobalMask() const;
	void addToGlobalMask() const;

	std::vector<Vec2> sourcePoints() const;
	std::vector<Vec2> sourceRotatedPoints() const;
	std::vector<Vec2> sourceRotatedPointsRelative2f() const;
	std::vector<Vec2> targetPoints() const;

	MondriaanPatch mutated(Mutation mutation, float step) const;
	float step(std::mt19937& generator, Mutation mutation) const;
};