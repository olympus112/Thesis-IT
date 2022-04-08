#include "core.h"

#include "utils.h"
#include "omp.h"

void Utils::matchTemplate(const cv::Mat& image, const cv::Mat& templ, cv::Mat& result, int method, const cv::Mat& templMask, const cv::Mat& globalMask) {
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
		abort:
			result.at<float>(colStart + rowStart * resultRows) = 0;
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

std::vector<std::size_t> Utils::nUniqueSampledSizeTypesInRange(std::mt19937& generator, std::size_t n, std::size_t start, std::size_t end, float(* pdf)(float)) {
	assert(n <= end - start);

	std::vector<std::size_t> result(n);
	for (std::size_t i = 0; i < n; i++) {
		std::size_t value = static_cast<std::size_t>(static_cast<float>(start) + pdf(Utils::randomUnsignedFloatInRange(generator, 0.0f, 1.0f) * static_cast<float>(end - start)));
		result.push_back(value);
	}

	return result;
}

std::vector<int> Utils::nUniqueSampledIntegersInRange(std::mt19937& generator, int n, int start, int end, float(* pdf)(float)) {
	assert(n <= end - start);
	assert(end >= 0 && start >= 0 && n > 0);

	std::vector<int> result(n);
	for (int i = 0; i < n; i++) {
		int value = static_cast<int>(static_cast<float>(start) + pdf(Utils::randomUnsignedFloatInRange(generator, 0.0f, 1.0f) * static_cast<float>(end - start)));
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
