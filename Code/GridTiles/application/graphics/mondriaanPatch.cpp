#include "core.h"
#include "mondriaanPatch.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "main.h"

MondriaanPatch::MondriaanPatch()
	: rotationIndex(0)
	, match(0) {
}

MondriaanPatch::MondriaanPatch(const Vec2f& sourceOffset, const Vec2f& targetOffset, const Vec2f& dimension) {
	this->sourceOffset = sourceOffset;
	this->targetOffset = targetOffset;
	this->dimension_mm = dimension;
	this->rotationIndex = 0;
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

	float rotation = 360.0f / settings.rotations * rotationIndex;
	auto rotated = [&](const Vec2& point) -> ImVec2 {
		return source.toAbsoluteScreenSpace(sourceRotatedBoundsCenter - sourceBoundsCenter + point.rotated(rotation, sourceBoundsCenter)).iv();
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

Bounds MondriaanPatch::targetBounds() const {
	return Boundsi(targetOffset, targetDimension());
}

Bounds MondriaanPatch::targetBoundsRelative() const {
	return Boundsi(Vec2(), targetDimension());
}

Vec2 MondriaanPatch::sourceDimension() const {
	return settings.smm2px(dimension_mm);
}

Vec2 MondriaanPatch::targetDimension() const {
	return settings.tmm2px(dimension_mm);
}

Bounds MondriaanPatch::sourceUV() const {
	return settings.sourcer.textures[rotationIndex].bounds().subBoundsUV(sourceBounds());
}

Bounds MondriaanPatch::sourceRotatedUV() const {
	return settings.sourcer.textures[rotationIndex].bounds().subBoundsUV(sourceRotatedBounds());
}

Bounds MondriaanPatch::targetUV() const {
	return settings.target->bounds().subBoundsUV(targetBounds());
}

// Computes the match with the current settings, based on the features and their distribution and the specified norm function. This required mask to be updated.
void MondriaanPatch::computeMatch() {
	//Vec2 targetDimension = this->targetDimension();
	//Vec2i rotatedSourceDimension(mask.cols, mask.rows);
	//cv::Rect rotatedSourceRegion(sourceOffset.cv(), sourceOffset.cv() + rotatedSourceDimension.cv());
	//cv::Rect targetRegion(targetOffset.cv(), targetOffset.cv() + targetDimension.cv());

	//float distribution[2];
	//distribution[FeatureIndex_Intensity] = settings.intensityWeight;
	//distribution[FeatureIndex_Edge] = settings.edgeWeight;

	//double value = 0.0;
	//for (FeatureIndex featureIndex = 0; featureIndex < Feature::get.size(); ++featureIndex) {
	//	// Inverse transform the source feature
	//	cv::Mat inverseRotatedSourceFeature;
	//	cv::Mat rotatedSourceFeature = settings.sourcer.features[rotationIndex][featureIndex].data(rotatedSourceRegion);
	//	cv::warpAffine(rotatedSourceFeature, inverseRotatedSourceFeature, inverseTransformationMatrix, targetDimension.cv());

	//	// Get target feature
	//	cv::Mat targetFeature = settings.target.features[featureIndex].data(targetRegion);

	//	value += distribution[featureIndex] * cv::norm(inverseRotatedSourceFeature, targetFeature, cv::NORM_L2 | cv::NORM_RELATIVE) / static_cast<
	//		double>(targetFeature.rows * targetFeature.cols);
	//}

	//this->match = value;
}

// Computes the best source location based on the current settings, rotation, features and their distribution using cv::MatchTemplate. Modifies the source location
void MondriaanPatch::computeBestMatch(cv::TemplateMatchModes metric) {
//	Boundsi targetBounds = this->targetBounds();
//	Vec2i sourceRotatedDimension = this->sourceRotatedDimension();
//	float distribution[2];
//	distribution[FeatureIndex_Intensity] = settings.intensityWeight;
//	distribution[FeatureIndex_Edge] = settings.edgeWeight;
//
//	// Collect feature responses
//	int featureCols = settings.sourcer->cols() - sourceRotatedDimension.x + 1;
//	int featureRows = settings.sourcer->rows() - sourceRotatedDimension.y + 1;
//	cv::Mat weightedResponse(featureRows, featureCols, CV_32F, cv::Scalar(0.0));
//
////#pragma omp parallel for
//	for (FeatureIndex featureIndex = 0; featureIndex < settings.target.features.size(); featureIndex++) {
//		// Get source and target feature patches
//		cv::Mat sourceFeature = settings.sourcer.features[rotationIndex][featureIndex].data;
//		cv::Mat targetFeaturePatch = settings.target[featureIndex].data(targetBounds.cv());
//
//		//assert(sourceFeature.cols == settings.source->cols());
//		//assert(sourceFeature.rows == settings.source->rows());
//
//		// Calculate rotated feature patch
//		cv::Mat rotatedTargetFeaturePatch;
//		cv::warpAffine(targetFeaturePatch, rotatedTargetFeaturePatch, transformationMatrix, sourceRotatedDimension.cv());
//
//		if (mask.cols != sourceRotatedDimension.x || mask.rows != sourceRotatedDimension.y) {
//			cv::Mat maskk = mask.clone();
//			std::vector<Vec2> points = sourceRotatedPointsRelative2f();
//			cv::line(maskk, points[0].cv(), points[1].cv(), Colors::RED.cv());
//			cv::line(maskk, points[1].cv(), points[2].cv(), Colors::RED.cv());
//			cv::line(maskk, points[2].cv(), points[3].cv(), Colors::RED.cv());
//			cv::line(maskk, points[3].cv(), points[0].cv(), Colors::RED.cv());
//			cv::imshow("maskk", maskk);
//			cv::waitKey();
//		}
//
//		// Find best match
//		cv::Mat response;
//		cv::matchTemplate(sourceFeature, rotatedTargetFeaturePatch, response, metric, mask.clone());
//
//		// Add weighted response to total
//#pragma omp critical
//		cv::addWeighted(weightedResponse, 1.0, response, distribution[featureIndex], 0.0, weightedResponse);
//	}
//
//	// Find min or max value
//	double value;
//	cv::Point point;
//
//	//cv::Mat filteredMaskRegion;
//	//cv::boxFilter(settings.mask.data, filteredMaskRegion, -1, sourceRotatedDimension.cv(), cv::Point(0, 0), true, cv::BORDER_ISOLATED);
//	//cv::threshold(filteredMaskRegion, filteredMaskRegion, 254, 255, cv::THRESH_BINARY);
//
//	//cv::imshow("mask", settings.mask.data);
//	//cv::imshow("filteredMaskRegion", filteredMaskRegion);
//	//cv::imshow("filteredMaskRegion2", filteredMaskRegion(cv::Rect(0, 0, weightedResponse.cols, weightedResponse.rows)));
//	//cv::imshow("weightedResponse", weightedResponse);
//	//cv::waitKey();
//
//	if (metric == cv::TM_SQDIFF || metric == cv::TM_SQDIFF_NORMED) {
//		cv::minMaxLoc(weightedResponse,
//		              &value,
//		              nullptr,
//		              &point,
//		              nullptr
//		              //filteredMaskRegion(cv::Rect(0, 0, weightedResponse.cols, weightedResponse.rows))
//		);
//	} else {
//		cv::minMaxLoc(weightedResponse,
//		              nullptr,
//		              &value,
//		              nullptr,
//		              &point
//		              //filteredMaskRegion(cv::Rect(0, 0, weightedResponse.cols, weightedResponse.rows))
//			);
//	}
//
//	if (point.x == -1 && point.y == -1) {
//		Log::error("No patches available");
//		/*cv::imshow("Mask", settings.mask.data);
//		cv::Mat filteredMaskRegion;
//		cv::boxFilter(settings.mask.data,
//		              filteredMaskRegion,
//		              -1,
//		              cv::Size(targetBounds.dimension().x, targetBounds.dimension().y),
//		              cv::Point(0, 0),
//		              true,
//		              cv::BORDER_ISOLATED);
//		cv::threshold(filteredMaskRegion, filteredMaskRegion, 254, 255, cv::THRESH_BINARY);
//		cv::imshow("Filtered mask", filteredMaskRegion);
//		cv::waitKey();*/
//
//		return;
//	}
//
//	// Move seedpoint
//	this->sourceOffset = Vec2(point.x, point.y);
//	this->match = value;
}


// Requires mask to be updated
void MondriaanPatch::removeFromGlobalMask() const {
	/*cv::Mat roi = settings.mask.data(sourceRotatedBounds().cv());
	cv::bitwise_or(roi, mask, roi);*/
}


void MondriaanPatch::addToGlobalMask() const {
	/*cv::Mat roi = settings.mask.data(sourceRotatedBounds().cv());
	cv::Mat invertedMask;
	cv::bitwise_not(mask, invertedMask);
	cv::bitwise_and(roi, invertedMask, roi);*/
}

std::vector<Vec2> MondriaanPatch::targetPoints() const {
	return targetBounds().ipoints();
}

std::vector<Vec2> MondriaanPatch::sourcePoints() const {
	return sourceBounds().ipoints();
}

Vec2 MondriaanPatch::sourceRotatedDimension() const {
	float rotation = 360.0 / settings.rotations * rotationIndex;
	cv::Size size = RotatedTexture::computeRotatedRect(sourceDimension().cv(), rotation).size();

	return Vec2(size.width, size.height);
}

Vec2 MondriaanPatch::sourceRotatedDimension2f() const {
	float rotation = 360.0 / settings.rotations * rotationIndex;

	cv::Size2f size = RotatedTexture::computeRotatedRect2f(sourceDimension().cv(), rotation).size();

	return Vec2(size.width, size.height);
}


Bounds MondriaanPatch::sourceRotatedBounds() const {
	return Bounds(sourceOffset, sourceRotatedDimension());
}

std::vector<Vec2> MondriaanPatch::sourceRotatedPoints() const {
	Bounds sourceBounds = this->sourceBounds();
	Vec2 sourceBoundsCenter = sourceBounds.center();
	Vec2 sourceRotatedBoundsCenter = this->sourceRotatedBounds().center();
	float rotation = 360.0f / settings.rotations * rotationIndex;

	auto rotated = [&](const Vec2& point) {
		return sourceRotatedBoundsCenter - sourceBoundsCenter + point.rotated(rotation, sourceBoundsCenter);
	};

	return std::vector{rotated(sourceBounds[0]), rotated(sourceBounds[1]), rotated(sourceBounds[2]), rotated(sourceBounds[3])};
}

std::vector<Vec2> MondriaanPatch::sourceRotatedPointsRelative2f() const {
	Bounds sourceBounds = this->sourceBoundsRelative();
	Vec2 sourceBoundsCenter = sourceBounds.center();
	Vec2 sourceRotatedBoundsCenter = this->sourceRotatedDimension2f() / 2;
	float rotation = 360.0f / settings.rotations * rotationIndex;

	auto rotated = [&] (const Vec2& point) {
		return sourceRotatedBoundsCenter - sourceBoundsCenter + point.rotated(rotation, sourceBoundsCenter);
	};

	return std::vector { rotated(sourceBounds[0]), rotated(sourceBounds[1]), rotated(sourceBounds[2]), rotated(sourceBounds[3]) };
}