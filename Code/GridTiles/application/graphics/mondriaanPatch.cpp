#include "core.h"
#include "mondriaanPatch.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "main.h"

MondriaanPatch::MondriaanPatch()
	: sourceRotation(0)
	, match(0) {
}

MondriaanPatch::MondriaanPatch(const Vec2f& sourceOffset, const Vec2f& targetOffset, const Vec2f& dimension, float sourceRotation) {
	this->sourceOffset = sourceOffset;
	this->targetOffset = targetOffset;
	this->dimension_mm = dimension;
	this->sourceRotation = sourceRotation;

	computeMask();
	computeTransformationMatrix();
}

void MondriaanPatch::render(const Canvas& source,
                            const Canvas& target,
                            bool intersected,
                            bool selected,
                            bool showConnections,
                            const Color& sourceColor,
                            const Color& targetColor) const {
	Bounds sourceBounds = this->sourceBounds();
	Bounds sourceRotatedBounds = this->sourceRotatedBounds();
	Bounds targetBounds = this->targetBounds();

	Vec2 targetBoundsCenter = targetBounds.center();
	Vec2 sourceBoundsCenter = sourceBounds.center();
	Vec2 sourceRotatedBoundsCenter = sourceRotatedBounds.center();

	auto rotated = [&](const Vec2& point) -> ImVec2 {
		return source.toAbsoluteScreenSpace(sourceRotatedBoundsCenter - sourceBoundsCenter + point.rotated(sourceRotation, sourceBoundsCenter)).iv();
	};

	ImVec2 tl = rotated(sourceBounds.tl());
	ImVec2 bl = rotated(sourceBounds.ebl());
	ImVec2 br = rotated(sourceBounds.ebr());
	ImVec2 tr = rotated(sourceBounds.etr());

	ImGui::GetWindowDrawList()->AddQuad(tl, bl, br, tr, sourceColor.u32(), intersected ? 3 : selected ? 2 : 1);
	ImGui::GetWindowDrawList()->AddRect(target.toAbsoluteScreenSpace(targetBounds.min()).iv(),
	                                    target.toAbsoluteScreenSpace(targetBounds.emax()).iv(),
	                                    targetColor.u32(),
	                                    0,
	                                    ImDrawCornerFlags_All,
	                                    intersected ? 3 : selected ? 2 : 1);
	if (selected)
		ImGui::GetWindowDrawList()->AddRect(source.toAbsoluteScreenSpace(sourceRotatedBounds.min()).iv(),
		                                    source.toAbsoluteScreenSpace(sourceRotatedBounds.emax()).iv(),
		                                    sourceColor.u32(),
		                                    0,
		                                    ImDrawCornerFlags_All,
		                                    intersected ? 3 : selected ? 2 : 1);

	if (showConnections || selected)
		ImGui::GetWindowDrawList()->AddLine(source.toAbsoluteScreenSpace(sourceRotatedBoundsCenter).iv(),
		                                    target.toAbsoluteScreenSpace(targetBoundsCenter).iv(),
		                                    Colors::WHITE.u32(),
		                                    intersected ? 3 : selected ? 2 : 1);
}

Bounds MondriaanPatch::sourceBounds() const {
	return Boundsi(sourceOffset, sourceDimension());
}

Bounds MondriaanPatch::sourceBoundsRelative() const {
	return Boundsi(Vec2(), sourceDimension());
}

Bounds MondriaanPatch::sourceRotatedBounds() const {
	/*Vec2 sourceDimension = this->sourceDimension();
	cv::Point center(sourceDimension.cv() / 2.0);
	cv::RotatedRect rotatedRect(center, cv::Size(sourceDimension.x, sourceDimension.y), sourceRotation);
	cv::Rect bounds = rotatedRect.boundingRect();
	bounds += cv::Point(cv::abs(bounds.x), cv::abs(bounds.y));*/

	return Bounds(sourceOffset, sourceRotatedDimension());
}

Bounds MondriaanPatch::sourceRotatedBoundsRelative() const {
	return Bounds(Vec2(), sourceRotatedDimension());
}

Bounds MondriaanPatch::sourceRotatedBoundsRelative2f() const {
	return Bounds(Vec2(), sourceRotatedDimension2f());
}

Bounds MondriaanPatch::targetBounds() const {
	return Boundsi(targetOffset, targetDimension());
}

Bounds MondriaanPatch::targetBoundsRelative() const {
	return Boundsi(Vec2(), targetDimension());
}

Vec2 MondriaanPatch::sourceDimension() const {
	return settings.smm2px(dimension_mm);
}

// Requires mask to be updated!
Vec2 MondriaanPatch::sourceRotatedDimension2f() const {
	cv::Size2f size = RotatedTexture::computeRotatedRect2f(sourceDimension().cv(), sourceRotation).size();

	return Vec2(size.width, size.height);
}

Vec2 MondriaanPatch::sourceRotatedDimension() const {
	cv::Size size = RotatedTexture::computeRotatedRect(sourceDimension().cv(), sourceRotation).size();

	return Vec2(size.width, size.height);
}


Vec2 MondriaanPatch::targetDimension() const {
	return settings.tmm2px(dimension_mm);
}

Bounds MondriaanPatch::sourceUV() const {
	return settings.source->bounds().subBoundsUV(sourceBounds());
}

Bounds MondriaanPatch::sourceRotatedUV() const {
	return settings.source->bounds().subBoundsUV(sourceRotatedBounds());
}

Bounds MondriaanPatch::targetUV() const {
	return settings.target->bounds().subBoundsUV(targetBounds());
}

// Computes the match with the current settings, based on the features and their distribution and the specified norm function. This required mask to be updated.
void MondriaanPatch::computeMatch() {
	Vec2 targetDimension = this->targetDimension();
	Vec2i rotatedSourceDimension(mask.cols, mask.rows);
	cv::Rect rotatedSourceRegion(sourceOffset.cv(), sourceOffset.cv() + rotatedSourceDimension.cv());
	cv::Rect targetRegion(targetOffset.cv(), targetOffset.cv() + targetDimension.cv());

	float distribution[2];
	distribution[FeatureIndex_Intensity] = settings.intensityWeight;
	distribution[FeatureIndex_Edge] = settings.edgeWeight;

	double value = 0.0;
	for (FeatureIndex featureIndex = 0; featureIndex < Feature::get.size(); ++featureIndex) {
		// Inverse transform the source feature
		cv::Mat inverseRotatedSourceFeature;
		cv::Mat rotatedSourceFeature = settings.source.features[featureIndex].data(rotatedSourceRegion);
		cv::warpAffine(rotatedSourceFeature, inverseRotatedSourceFeature, inverseTransformationMatrix, targetDimension.cv());

		// Get target feature
		cv::Mat targetFeature = settings.target.features[featureIndex].data(targetRegion);

		value += distribution[featureIndex] * cv::norm(inverseRotatedSourceFeature, targetFeature, cv::NORM_L2 | cv::NORM_RELATIVE) / static_cast<
			double>(targetFeature.rows * targetFeature.cols);
	}

	this->match = value;
}

// Computes the best source location based on the current settings, rotation, features and their distribution using cv::MatchTemplate. Modifies the source location
void MondriaanPatch::computeBestMatch(cv::TemplateMatchModes metric) {
	Boundsi targetBounds = this->targetBounds();
	Vec2i sourceRotatedDimension = this->sourceRotatedDimension();
	float distribution[2];
	distribution[FeatureIndex_Intensity] = settings.intensityWeight;
	distribution[FeatureIndex_Edge] = settings.edgeWeight;

	// Collect feature responses
	int featureCols = settings.source->cols() - sourceRotatedDimension.x + 1;
	int featureRows = settings.source->rows() - sourceRotatedDimension.y + 1;
	cv::Mat weightedResponse(featureRows, featureCols, CV_32F, cv::Scalar(0.0));

#pragma omp parallel for
	for (FeatureIndex featureIndex = 0; featureIndex < settings.target.features.size(); featureIndex++) {
		// Get source and target feature patches
		cv::Mat sourceFeature = settings.source[featureIndex].data;
		cv::Mat targetFeaturePatch = settings.target[featureIndex].data(targetBounds.cv());

		assert(sourceFeature.cols == settings.source->cols());
		assert(sourceFeature.rows == settings.source->rows());

		// Calculate rotated feature patch
		cv::Mat rotatedTargetFeaturePatch;
		cv::warpAffine(targetFeaturePatch, rotatedTargetFeaturePatch, transformationMatrix, sourceRotatedDimension.cv());

		if (mask.cols != sourceRotatedDimension.x || mask.rows != sourceRotatedDimension.y) {
			cv::Mat maskk = mask.clone();
			std::vector<Vec2> points = sourceRotatedPointsRelative2f();
			cv::line(maskk, points[0].cv(), points[1].cv(), Colors::RED.cv());
			cv::line(maskk, points[1].cv(), points[2].cv(), Colors::RED.cv());
			cv::line(maskk, points[2].cv(), points[3].cv(), Colors::RED.cv());
			cv::line(maskk, points[3].cv(), points[0].cv(), Colors::RED.cv());
			cv::imshow("maskk", maskk);
			cv::waitKey();
		}

		// Find best match
		cv::Mat response;
		cv::matchTemplate(sourceFeature, rotatedTargetFeaturePatch, response, metric, mask.clone());

		// Add weighted response to total
#pragma omp critical
		cv::addWeighted(weightedResponse, 1.0, response, distribution[featureIndex], 0.0, weightedResponse);
	}

	// Find min or max value
	double value;
	cv::Point point;

	//cv::Mat filteredMaskRegion;
	//cv::boxFilter(settings.mask.data, filteredMaskRegion, -1, sourceRotatedDimension.cv(), cv::Point(0, 0), true, cv::BORDER_ISOLATED);
	//cv::threshold(filteredMaskRegion, filteredMaskRegion, 254, 255, cv::THRESH_BINARY);

	//cv::imshow("mask", settings.mask.data);
	//cv::imshow("filteredMaskRegion", filteredMaskRegion);
	//cv::imshow("filteredMaskRegion2", filteredMaskRegion(cv::Rect(0, 0, weightedResponse.cols, weightedResponse.rows)));
	//cv::imshow("weightedResponse", weightedResponse);
	//cv::waitKey();

	if (metric == cv::TM_SQDIFF || metric == cv::TM_SQDIFF_NORMED) {
		cv::minMaxLoc(weightedResponse,
		              &value,
		              nullptr,
		              &point,
		              nullptr
		              //filteredMaskRegion(cv::Rect(0, 0, weightedResponse.cols, weightedResponse.rows))
		);
	} else {
		cv::minMaxLoc(weightedResponse,
		              nullptr,
		              &value,
		              nullptr,
		              &point
		              //filteredMaskRegion(cv::Rect(0, 0, weightedResponse.cols, weightedResponse.rows))
			);
	}

	if (point.x == -1 && point.y == -1) {
		Log::error("No patches available");
		/*cv::imshow("Mask", settings.mask.data);
		cv::Mat filteredMaskRegion;
		cv::boxFilter(settings.mask.data,
		              filteredMaskRegion,
		              -1,
		              cv::Size(targetBounds.dimension().x, targetBounds.dimension().y),
		              cv::Point(0, 0),
		              true,
		              cv::BORDER_ISOLATED);
		cv::threshold(filteredMaskRegion, filteredMaskRegion, 254, 255, cv::THRESH_BINARY);
		cv::imshow("Filtered mask", filteredMaskRegion);
		cv::waitKey();*/

		return;
	}

	// Move seedpoint
	this->sourceOffset = Vec2(point.x, point.y);
	this->match = value;
}

void MondriaanPatch::computeMask() {
	this->mask = Mask::rotated(sourceDimension(), sourceRotation);
}

void MondriaanPatch::computeTransformationMatrix() {
	this->transformationMatrix = RotatedTexture::computeTransformationMatrix(sourceDimension().cv(), sourceRotation);
	cv::invertAffineTransform(this->transformationMatrix, this->inverseTransformationMatrix);
}

// Requires mask to be updated
void MondriaanPatch::removeFromGlobalMask() const {
	cv::Mat roi = settings.mask.data(sourceRotatedBounds().cv());
	cv::bitwise_or(roi, mask, roi);
}


void MondriaanPatch::addToGlobalMask() const {
	cv::Mat roi = settings.mask.data(sourceRotatedBounds().cv());
	cv::Mat invertedMask;
	cv::bitwise_not(mask, invertedMask);
	cv::bitwise_and(roi, invertedMask, roi);
}

std::vector<Vec2> MondriaanPatch::targetPoints() const {
	return targetBounds().ipoints();
}

std::vector<Vec2> MondriaanPatch::sourcePoints() const {
	return sourceBounds().ipoints();
}

std::vector<Vec2> MondriaanPatch::sourceRotatedPoints() const {
	Bounds sourceBounds = this->sourceBounds();
	Vec2 sourceBoundsCenter = sourceBounds.center();
	Vec2 sourceRotatedBoundsCenter = this->sourceRotatedBounds().center();

	auto rotated = [&](const Vec2& point) {
		return sourceRotatedBoundsCenter - sourceBoundsCenter + point.rotated(sourceRotation, sourceBoundsCenter);
	};

	return std::vector{rotated(sourceBounds[0]), rotated(sourceBounds[1]), rotated(sourceBounds[2]), rotated(sourceBounds[3])};
}

std::vector<Vec2> MondriaanPatch::sourceRotatedPointsRelative2f() const {
	Bounds sourceBounds = this->sourceBoundsRelative();
	Vec2 sourceBoundsCenter = sourceBounds.center();
	Vec2 sourceRotatedBoundsCenter = this->sourceRotatedBoundsRelative2f().center();

	auto rotated = [&] (const Vec2& point) {
		return sourceRotatedBoundsCenter - sourceBoundsCenter + point.rotated(sourceRotation, sourceBoundsCenter);
	};

	return std::vector { rotated(sourceBounds[0]), rotated(sourceBounds[1]), rotated(sourceBounds[2]), rotated(sourceBounds[3]) };
}

MondriaanPatch MondriaanPatch::mutated(Mutation mutation, float step) const {
	MondriaanPatch result;
	result.sourceOffset = sourceOffset;
	result.targetOffset = targetOffset;
	result.dimension_mm = dimension_mm;
	result.sourceRotation = sourceRotation;

	bool maskDirty = false;

	switch (mutation) {
	case Mutation_TargetMoveHorizontal:
		result.targetOffset.x += step;
		break;
	case Mutation_TargetMoveVertical:
		result.targetOffset.y -= step;
		break;
	case Mutation_ResizeLeft:
		result.targetOffset.x -= step;
		result.dimension_mm.x += settings.tpx2mm(step);
		maskDirty = true;
		break;
	case Mutation_ResizeRight:
		result.dimension_mm.x += settings.tpx2mm(step);
		maskDirty = true;
		break;
	case Mutation_ResizeUp:
		result.targetOffset.y -= step;
		result.dimension_mm.y += settings.tpx2mm(step);
		maskDirty = true;
		break;
	case Mutation_ResizeDown:
		result.dimension_mm.y += settings.tpx2mm(step);
		maskDirty = true;
		break;
	case Mutation_Rotate:
		result.sourceRotation += step;
		maskDirty = true;
		break;
	}

	if (maskDirty) {
		result.computeTransformationMatrix();
		result.computeMask();
	} else {
		result.mask = mask.clone();
		result.transformationMatrix = transformationMatrix.clone();
		result.inverseTransformationMatrix = inverseTransformationMatrix.clone();
	}

	Log::print("%s with step %.2f", mutationNames[mutation], step);

	return result;
}

float MondriaanPatch::step(std::mt19937& generator, Mutation mutation) const {
	switch (mutation) {
	case Mutation_TargetMoveHorizontal:
	case Mutation_TargetMoveVertical:
		return Utils::randomSignedFloatInRange(generator, 0.8f, 2.2f, 0.5f);
	case Mutation_ResizeLeft:
	case Mutation_ResizeRight:
	case Mutation_ResizeUp:
	case Mutation_ResizeDown:
		return Utils::randomSignedFloatInRange(generator, 2.0, 5.0, 0.0f);
	case Mutation_Rotate:
		return Utils::randomSignedFloatInRange(generator, 1.5f, 1.5f, 0.5f);
	}

	return 1.0f;
}
