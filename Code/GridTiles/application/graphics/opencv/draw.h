#pragma once

#include <opencv2/opencv.hpp>

namespace CV {
	cv::Mat drawBars(const std::vector<cv::Mat>& bars, int size = 512);
	cv::Mat drawLines(const std::vector<cv::Mat>& lines, int size = 512);
}
