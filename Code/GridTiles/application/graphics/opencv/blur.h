#pragma once

#include <opencv2/opencv.hpp>

class Blur {
public:
	Vec2 sigma;
	cv::Size size;
	cv::Mat blur;

	Blur(const cv::Mat& texture, const cv::Size& size = {3, 3}, const Vec2& sigma = { 0, 0 });

	static cv::Mat computeBlur(const cv::Mat& source, const cv::Size& size, const Vec2& sigma);
};
