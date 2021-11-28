#include "core.h"
#include "sobel.h"

cv::Mat Sobel::computeSobel(const cv::Mat& source, const SobelType& type, int derivative, int size) {
	cv::Mat result;

	int dx = type == SobelType::X || type == SobelType::XY ? derivative : 0;
	int dy = type == SobelType::Y || type == SobelType::XY ? derivative : 0;
	cv::Sobel(source, result, CV_8U, dx, dy, size);

	return result;
}

Sobel::Sobel(const cv::Mat& texture, const SobelType& type) {
	this->type = type;
	this->sobel = computeSobel(texture, type, 1, 5);
}

