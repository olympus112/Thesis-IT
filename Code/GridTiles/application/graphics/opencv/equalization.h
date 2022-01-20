#pragma once
#include <opencv2/opencv.hpp>

#include "cdf.h"
#include "graphics/textures/texture.h"

class Equalization {
public:
	int nChannels;
	std::vector<std::vector<int>> table;
	cv::Mat equalization;

	Equalization(Texture* texture, const CDF& source, const CDF& reference);
	Equalization(Texture* texture, const Histogram& source, const Histogram& reference);
	Equalization(Texture* texture, Texture* reference);
	Equalization(const cv::Mat& texture, const CDF& source, const CDF& reference);
	Equalization(const cv::Mat& texture, const Histogram& source, const Histogram& reference);
	Equalization(const cv::Mat& texture, const cv::Mat& reference);

	static std::vector<std::vector<int>> computeEqualizationTable(const CDF& source,
		const CDF& reference);
	static cv::Mat computeEqualization(const std::vector<cv::Mat>& channels, std::vector<std::vector<int>> table);
};
