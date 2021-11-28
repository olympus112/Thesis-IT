#pragma once

#include <opencv2/opencv.hpp>
#include "histogram.h"

class CDF {
public:
	int nBins;
	int nChannels;
	std::vector<cv::Mat> cdf;

	CDF(const std::vector<cv::Mat>& histograms, int nBins);
	CDF(const Histogram& histogram);

	cv::Mat draw();

	const cv::Mat& operator[](int channel) const;

	static std::vector<cv::Mat> computeCDF(const std::vector<cv::Mat>& histograms, int nBins, int size);
};
