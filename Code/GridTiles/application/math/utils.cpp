#include "core.h"

#include "utils.h"

#include <opencv2/highgui.hpp>

#include "omp.h"

#include <opencv2/core/mat.hpp>
#include "main.h"
#include "graphics/features/Feature.h"

void Utils::matchTemplate(const cv::Mat& image,
                          const cv::Mat& templ,
                          cv::Mat& result,
                          int method,
                          const cv::Mat& templMask,
                          const cv::Mat& globalMask) {
	int templCols = templ.cols;
	int templRows = templ.rows;
	int resultCols = image.cols - templ.cols + 1;
	int resultRows = image.rows - templ.rows + 1;

	bool hasTemplMask = templMask.data != nullptr;
	bool hasGlobalMask = globalMask.data != nullptr;

	result.create(resultRows, resultCols, CV_32FC1);

#pragma omp parallel for
	for (int colStart = 0; colStart < resultCols; ++colStart) {
#pragma omp parallel for
		for (int rowStart = 0; rowStart < resultRows; ++rowStart) {
			float totalPixels = 0.0;
			float resultValue = 0.0;
			for (int colDelta = 0; colDelta < templCols; colDelta++) {
				for (int rowDelta = 0; rowDelta < templRows; rowDelta++) {
					int globalCol = colStart + colDelta;
					int globalRow = rowStart + rowDelta;
					int templIndex = colDelta + rowDelta * templCols;
					int globalIndex = globalCol + globalRow * resultCols;

					if (hasTemplMask && templMask.at<unsigned char>(templIndex) == 0)
						continue;

					if (hasGlobalMask && globalMask.at<unsigned char>(globalIndex) == 0)
						goto abort;

					int globalValue = image.at<unsigned char>(globalIndex);
					int templValue = templ.at<unsigned char>(templIndex);

					resultValue += static_cast<float>(globalValue * templValue);
					totalPixels += 1.0;
				}
			}

			result.at<float>(colStart + rowStart * resultRows) = resultValue / totalPixels;

			continue;
		abort: result.at<float>(colStart + rowStart * resultRows) = 0;
		}
	}
}

std::vector<std::size_t> Utils::nUniqueRandomSizeTypesInRange(std::mt19937& generator, std::size_t n, std::size_t start, std::size_t end) {
	assert(n <= end - start);

	std::vector<std::size_t> result(end - start);
	for (std::size_t i = start; i < end; i++)
		result[i] = i;

	std::ranges::shuffle(result, generator);

	return std::vector(result.begin(), result.begin() + n);
}

std::vector<std::size_t> Utils::nUniqueSampledSizeTypesInRange(std::mt19937& generator,
                                                               std::size_t n,
                                                               std::size_t start,
                                                               std::size_t end,
                                                               float (* pdf)(float)) {
	assert(n <= end - start);

	std::vector<std::size_t> result(n);
	for (std::size_t i = 0; i < n; i++) {
		std::size_t value = static_cast<std::size_t>(static_cast<float>(start) + pdf(
			Utils::randomUnsignedFloatInRange(generator, 0.0f, 1.0f) * static_cast<float>(end - start)));
		result[i] = value;
	}

	return result;
}

std::vector<int> Utils::nUniqueSampledIntegersInRange(std::mt19937& generator, int n, int start, int end, float (* pdf)(float)) {
	assert(n <= end - start);
	assert(end >= 0 && start >= 0 && n > 0);

	std::vector<int> result(n);
	for (int i = 0; i < n; i++) {
		int value = static_cast<int>(static_cast<float>(start) + pdf(
			Utils::randomUnsignedFloatInRange(generator, 0.0f, 1.0f) * static_cast<float>(end - start)));
		result.push_back(value);
	}

	return result;
}

std::vector<int> Utils::nUniqueRandomIntegersInRange(std::mt19937& generator, int n, int start, int end) {
	assert(n <= end - start);
	assert(end >= 0 && start >=0 && n > 0);

	std::vector<int> result(end - start);
	for (int i = start; i < end; i++)
		result[i] = i;

	std::ranges::shuffle(result, generator);

	return std::vector(result.begin(), result.begin() + n);
}

bool Utils::randomBool(std::mt19937& generator) {
	static std::uniform_int_distribution distribution(0, 1);
	return distribution(generator) == 0;
}

float Utils::randomUnsignedFloatInRange(std::mt19937& generator, float start, float end) {
	std::uniform_real_distribution distribution(start, end);

	return distribution(generator);
}

float Utils::randomSignedFloatInRange(std::mt19937& generator, float start, float end, float sign) {
	float number = randomUnsignedFloatInRange(generator, start, end);
	bool negative = randomUnsignedFloatInRange(generator, 0.0f, 1.0f) < sign;

	return negative ? -number : number;
}

std::pair<int, Vec2> Utils::computeBestMatch(const cv::Rect& patch, cv::TemplateMatchModes metric) {
	float distribution[2];
	distribution[FeatureIndex_Intensity] = settings.intensityWeight;
	distribution[FeatureIndex_Edge] = settings.edgeWeight;

	std::vector responses(settings.sourcer.rotations, cv::Mat());

#pragma omp parallel for
	for (int rotationIndex = 0; rotationIndex < settings.sourcer.rotations; rotationIndex++) {

		// Collect feature responses
		int featureCols = settings.sourcer.textures[rotationIndex].cols() - patch.width + 1;
		int featureRows = settings.sourcer.textures[rotationIndex].rows() - patch.height + 1;
		cv::Mat weightedResponse(featureRows, featureCols, CV_32F, cv::Scalar(0.0));
#pragma omp parallel for
		for (FeatureIndex featureIndex = 0; featureIndex < settings.sourcer.features[rotationIndex].size(); featureIndex++) {
			// Get source and target feature patches
			cv::Mat sourceFeature = settings.sourcer.features[rotationIndex][featureIndex].data;
			cv::Mat targetFeaturePatch = settings.target[featureIndex].data(patch);

			// Find best match
			cv::Mat response;
			cv::matchTemplate(sourceFeature, targetFeaturePatch, response, metric);

			// Add weighted response to total
#pragma omp critical
			cv::addWeighted(weightedResponse, 1.0, response, distribution[featureIndex], 0.0, weightedResponse);
		}

		responses[rotationIndex] = weightedResponse;
	}

	double bestValue;
	cv::Point bestPoint;
	int bestRotationIndex = 0;
	for (int rotationIndex = 0; rotationIndex < settings.sourcer.rotations; rotationIndex++) {
		cv::Mat mask;
		cv::boxFilter(settings.sourcer.masks[rotationIndex].data,
		              mask,
		              -1,
		              cv::Size(patch.width, patch.height),
		              cv::Point(0, 0),
		              true,
		              cv::BORDER_ISOLATED);
		cv::threshold(mask, mask, 254, 255, cv::THRESH_BINARY);
		//cv::imshow("Mask", mask); cv::waitKey();
		double value;
		cv::Point point;
		if (metric == cv::TM_SQDIFF || metric == cv::TM_SQDIFF_NORMED) {
			cv::minMaxLoc(responses[rotationIndex],
			              &value,
			              nullptr,
			              &point,
			              nullptr,
			              mask(cv::Rect(0, 0, responses[rotationIndex].cols, responses[rotationIndex].rows)));

			if (rotationIndex == 0 || value < bestValue) {
				bestValue = value;
				bestPoint = point;
				bestRotationIndex = rotationIndex;
			}
		} else {
			cv::minMaxLoc(responses[rotationIndex],
			              nullptr,
			              &value,
			              nullptr,
			              &point,
			              mask(cv::Rect(0, 0, responses[rotationIndex].cols, responses[rotationIndex].rows)));

			if (rotationIndex == 0 || value > bestValue) {
				bestValue = value;
				bestPoint = point;
				bestRotationIndex = rotationIndex;
			}
		}
	}

	if (bestPoint.x == -1 && bestPoint.y == -1) {
		Log::error("No patches available");

		return std::make_pair(0, Vec2(0, 0));
	}

	cv::Point transformedPoint = Utils::warp(bestPoint, settings.sourcer.inverseTransformations[bestRotationIndex]);

	if (transformedPoint.x > settings.sourcer->data.cols - patch.width - 1) {
		Log::debug("Too large x");
		transformedPoint.x = settings.sourcer->data.cols - patch.width - 1;
	}

	if (transformedPoint.y > settings.sourcer->data.rows - patch.height - 1) {
		Log::debug("Too large y");
		transformedPoint.y = settings.sourcer->data.rows - patch.height - 1;
	}

	// Move seedpoint
	return std::make_pair(bestRotationIndex, Vec2(bestPoint.x, bestPoint.y));
}

cv::Mat Utils::computeTransformationMatrix(const cv::Size& originalSize, double degrees) {
	const cv::Rect bounds = computeRotatedRect(originalSize, degrees);
	cv::Point2f center(0.5f * static_cast<float>(originalSize.width - 1), 0.5f * static_cast<float>(originalSize.height - 1));

	cv::Mat transformationMatrix = cv::getRotationMatrix2D(center, degrees, 1.0);
	transformationMatrix.at<double>(0, 2) += 0.5 * bounds.width - 0.5 * originalSize.width;
	transformationMatrix.at<double>(1, 2) += 0.5 * bounds.height - 0.5 * originalSize.height;

	return transformationMatrix;
}

cv::Mat Utils::computeTransformationMatrix(const cv::Size& originalSize, const cv::Size& rotatedSize, double degrees) {
	cv::Point2d center(0.5 * static_cast<double>(originalSize.width - 1), 0.5 * static_cast<double>(originalSize.height - 1));

	cv::Mat transformationMatrix = cv::getRotationMatrix2D(center, degrees, 1.0);
	transformationMatrix.at<double>(0, 2) += 0.5 * rotatedSize.width - 0.5 * originalSize.width;
	transformationMatrix.at<double>(1, 2) += 0.5 * rotatedSize.height - 0.5 * originalSize.height;

	return transformationMatrix;
}

cv::Rect Utils::computeRotatedRect(const cv::Size& originalSize, double degrees) {
	return cv::RotatedRect(cv::Point2d(), originalSize, degrees).boundingRect();
}

cv::Rect2f Utils::computeRotatedRect2f(const cv::Size& originalSize, double degrees) {
	return cv::RotatedRect(cv::Point2d(), originalSize, degrees).boundingRect2f();
}

cv::Point Utils::warp(cv::Point point, cv::Mat M) {
	return cv::Point(
		M.at<double>(0, 0) * point.x + M.at<double>(0, 1) * point.y + M.at<double>(0, 2),
		M.at<double>(1, 0) * point.x + M.at<double>(1, 1) * point.y + M.at<double>(1, 2)
	);
}
