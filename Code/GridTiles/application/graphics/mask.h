#pragma once

#include <opencv2/core.hpp>
#include "shape.h"

class Mask {
public:
	cv::Mat data;

	Mask() = default;
	Mask(const Shape& shape);
	Mask(const Vec2& dimension, double rotation);

	cv::Range rowRange(int rowOffset) const;
	cv::Range columnRange(int columnOffset) const;

	static cv::Mat copy(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset);
	static void cut(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset);
	static void paste(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset, const Color& color = Colors::WHITE);
	static cv::Mat fromPoints(const Bounds& bounds, const std::vector<Vec2>& points);
	static cv::Mat rotated(const Vec2& dimension, double rotation);
};