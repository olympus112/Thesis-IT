#include <core.h>
#include "SSPG_TemplateMatch.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "thread_pool/thread_pool.hpp"

#include "main.h"

void SSPG_TemplateMatch::renderSettings(Canvas& source, Canvas& target) {
	ImGui::SliderInt("Interdistance##Source", &interdistance, 10, 500);
	static std::array<const char*, 6> metrics = {
		"SQDIFF",
		"SQDIFF_NORMED",
		"CCORR",
		"CCORR_NORMED",
		"CCOEFF",
		"CCOEFF_NORMED",
	};
	ImGui::Combo("Distance metric", &metric, metrics.data(), metrics.size());
	ImGui::SliderInt("Rotations", &rotations, 1, 30);
}

void SSPG_TemplateMatch::mutate(std::vector<MondriaanPatch>& patches) {
	float distribution[2];
	distribution[FeatureIndex_Intensity] = settings.intensityWeight;
	distribution[FeatureIndex_Edge] = settings.edgeWeight;

	// Reset global mask
	settings.mask.data = cv::Mat(settings.source->rows(), settings.source->cols(), CV_8UC1, cv::Scalar(255));

	// Target feature mask array
	cv::Mat* rotatedTargetFeatureMasks = new cv::Mat[rotations];
	cv::Mat* patchFeatureResponses = new cv::Mat[patches.size()];

	// Thread pool
	thread_pool threadPool;

	for (MondriaanPatch& patch : patches) {
		Boundsi targetBounds = patch.targetBounds();

		// Compute non rotated mask
		cv::Mat originalTargetFeatureMask = cv::Mat(targetBounds.height(), targetBounds.width(), CV_8UC1, cv::Scalar(255));
		//cv::imshow("Original feature mask", originalTargetFeatureMask);

		cv::Point bestPoint;
		int bestRotationIndex;
		double bestValue = (metric == cv::TM_SQDIFF || metric == cv::TM_SQDIFF_NORMED) ? std::numeric_limits<double>::infinity() : 0;

		std::size_t featureCount = settings.target.features.size();

		//threadPool.parallelize_loop(0, rotations, [&] (const int& start, const int& end) {
#pragma omp parallel for
			for (int rotationIndex = /*start*/0; rotationIndex < /*end*/rotations; rotationIndex++) {
				cv::Size rotatedBounds;
				cv::Mat transformationMatrix;

				// Create rotated version of mask and bounds
				if (rotationIndex == 0) {
					rotatedBounds = targetBounds.dimension().cv();
					rotatedTargetFeatureMasks[rotationIndex] = originalTargetFeatureMask.clone();
				} else {
					float degrees = 360.0f * static_cast<float>(rotationIndex) / static_cast<float>(rotations);

					transformationMatrix = RotatedTexture::computeTransformationMatrix(targetBounds.dimension().cv(), degrees);
					rotatedBounds = RotatedTexture::computeRotatedRect(targetBounds.dimension().cv(), degrees).size();
					cv::warpAffine(originalTargetFeatureMask, rotatedTargetFeatureMasks[rotationIndex], transformationMatrix, rotatedBounds);
					//cv::waitKey();
				}

				// Collect feature responses
				int featureCols = settings.source->cols() - rotatedBounds.width + 1;
				int featureRows = settings.source->rows() - rotatedBounds.height + 1;
				cv::Mat weightedResponse(featureRows, featureCols, CV_32F, cv::Scalar(0.0));

				#pragma omp parallel for
				for (int featureIndex = 0; featureIndex < featureCount; featureIndex++) {
					cv::Mat sourceFeature = settings.source[featureIndex].data;
					assert(sourceFeature.cols == settings.source->cols());
					assert(sourceFeature.rows == settings.source->rows());
					
					cv::Mat targetFeaturePatch = settings.target[featureIndex].data(targetBounds.cv());

					// Calculate rotated feature patch
					cv::Mat rotatedTargetFeaturePatch;
					if (rotationIndex == 0)
						rotatedTargetFeaturePatch = targetFeaturePatch;
					else
						cv::warpAffine(targetFeaturePatch, rotatedTargetFeaturePatch, transformationMatrix, rotatedBounds);

					// Find best match
					cv::Mat response;
					cv::matchTemplate(sourceFeature, rotatedTargetFeaturePatch, response, this->metric, rotatedTargetFeatureMasks[rotationIndex].clone());

					//cv::imshow("response", response);
					//cv::imshow("source feature", sourceFeature);
					//cv::imshow("rotated target feature patch", rotatedTargetFeaturePatch);
					//cv::imshow("rotated target feature mask", rotatedTargetFeatureMasks[rotationIndex]);
					//cv::waitKey();

					// Add weighted response to total
					cv::addWeighted(weightedResponse, 1.0, response, distribution[featureIndex], 0.0, weightedResponse);
				}

				// Find min or max value
				double value;
				cv::Point point;

				cv::Mat filteredMaskRegion;
				cv::boxFilter(settings.mask.data, filteredMaskRegion, -1, cv::Size(rotatedBounds.width, rotatedBounds.height), cv::Point(0, 0), true, cv::BORDER_ISOLATED);
				cv::threshold(filteredMaskRegion, filteredMaskRegion, 254, 255, cv::THRESH_BINARY);
				/*cv::imshow("Mask", filteredMaskRegion);
				cv::waitKey();*/
				if (metric == cv::TM_SQDIFF || metric == cv::TM_SQDIFF_NORMED) {
					//cv::imshow("Response", weightedResponse);
					//cv::waitKey();
					cv::minMaxLoc(weightedResponse, &value, nullptr, &point, nullptr, filteredMaskRegion(cv::Rect(0, 0, weightedResponse.cols, weightedResponse.rows)));
					if (value < bestValue) {
						bestValue = value;
						bestPoint = point;
						bestRotationIndex = rotationIndex;
					}
				} else {
					cv::minMaxLoc(weightedResponse, nullptr, &value, nullptr, &point, filteredMaskRegion(cv::Rect(0, 0, weightedResponse.cols, weightedResponse.rows)));
					if (value > bestValue) {
						bestValue = value;
						bestPoint = point;
						bestRotationIndex = rotationIndex;
					}
				}
			}
		//});

		if (bestPoint.x == -1 && bestPoint.y == -1) {
			Log::error("No patches available");
			cv::imshow("Mask", settings.mask.data);
			cv::Mat filteredMaskRegion;
			cv::boxFilter(settings.mask.data, filteredMaskRegion, -1, cv::Size(targetBounds.dimension().x, targetBounds.dimension().y), cv::Point(0, 0), true, cv::BORDER_ISOLATED);
			cv::threshold(filteredMaskRegion, filteredMaskRegion, 254, 255, cv::THRESH_BINARY);
			cv::imshow("Filtered mask", filteredMaskRegion);
			cv::waitKey();
			return;
		}

		// Set mask
		cv::Mat bestRotatedMaskInverse;
		cv::bitwise_not(rotatedTargetFeatureMasks[bestRotationIndex], bestRotatedMaskInverse);

		cv::Rect roi = cv::Rect(bestPoint.x, bestPoint.y, bestRotatedMaskInverse.cols, bestRotatedMaskInverse.rows);
		cv::bitwise_and(settings.mask.data(roi), bestRotatedMaskInverse, settings.mask.data(roi));

		// Move seedpoint
		patch.sourceOffset = Vec2(bestPoint.x, bestPoint.y);
		patch.sourceRotation = 360.0f * static_cast<float>(bestRotationIndex) / static_cast<float>(rotations);
		patch.mask = rotatedTargetFeatureMasks[bestRotationIndex].clone();
		patch.match = bestValue;
		patch.computeTransformationMatrix();
	}

	settings.mask.reloadGL();
}