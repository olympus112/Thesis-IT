#pragma once

#include <opencv2/opencv.hpp>

class Histogram {
public:
	int nBins;
	int nChannels;
	int rangeMin;
	int rangeMax;
	std::vector<cv::Mat> histogram;

	Histogram(const cv::Mat& texture, int nBins = 256);
	Histogram(const std::vector<cv::Mat>& channels, int nBins = 256);

	cv::Mat drawLines();
	cv::Mat drawBars();

	const cv::Mat& operator[](int channel) const;

	static std::vector<cv::Mat> computeHistogram(const std::vector<cv::Mat>& channels,
		int nBins, int rangeMin, int rangeMax, int size);
};

