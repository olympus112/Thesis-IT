#pragma once

#include <opencv2/opencv.hpp>

#include "graphics/textures/texture.h"

class Grayscale {
public:
	cv::Mat grayscale;

	Grayscale(Texture* texture);
	Grayscale(const cv::Mat& texture);

	static cv::Mat computeGrayscale(const cv::Mat& source);
};
