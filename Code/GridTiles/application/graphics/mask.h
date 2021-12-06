#pragma once

#include <opencv2/core.hpp>
#include "shape.h"

class Mask {
public:
	Vec2i offset;
	cv::Mat pixels;

	Mask() = default;
	Mask(const Shape& shape);

	cv::Range rowRange(int rowOffset) const;
	cv::Range columnRange(int columnOffset) const;

	static cv::Mat copy(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset);
	static void cut(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset);
	static void paste(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset, const Color& color = Colors::WHITE);
};

