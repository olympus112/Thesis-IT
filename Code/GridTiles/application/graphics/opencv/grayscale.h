#pragma once

#include <opencv2/opencv.hpp>

class Grayscale {
public:
	cv::Mat grayscale;

	Grayscale(const cv::Mat& texture);

	static cv::Mat computeGrayscale(const cv::Mat& source);
};
