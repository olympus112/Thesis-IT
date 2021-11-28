#pragma once

#include <opencv2/opencv.hpp>

class Canny {
public:
	double threshold1;
	double threshold2;
	int apertureSize;
	bool L2gradient;
	cv::Mat canny;

	Canny(const cv::Mat& texture, double threshold1 = 100, double threshold2 = 200,
		int apertureSize = 3, bool L2gradient = false);

	static cv::Mat computeCanny(const cv::Mat& source, double threshold1, double threshold2,
		int apertureSize = 3, bool L2gradient = false);
};
