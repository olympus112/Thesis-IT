#pragma once

#include <opencv2/opencv.hpp>

enum class SobelType {
	X,
	Y,
	XY,
	MAGNITUDE
};

class Sobel {
public:
	cv::Mat sobel;
	SobelType type;

	Sobel(const cv::Mat& texture, const SobelType& type, int derivative = 1, int size = 3);

	static cv::Mat computeSobel(const cv::Mat& source, const SobelType& type, int derivative, int size);
};
