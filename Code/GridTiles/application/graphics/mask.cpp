#include "core.h"
#include "mask.h"

#include <opencv2/imgproc.hpp>

Mask::Mask(const Shape& shape) {
	Boundsi bounds = shape.bounds;

	std::vector<cv::Point> points(shape.size());
	for (std::size_t index = 0; index < shape.size(); index++) {
		cv::Point point = shape[index].asCV();
		cv::Point min = bounds.min().asCV();
		cv::Point result = point - min;
		points[index] = result;
	}

	this->offset = bounds.min();
	this->pixels = cv::Mat(bounds.height(), bounds.width(), CV_8U, cv::Scalar(0));

	cv::fillConvexPoly(this->pixels, points.data(), static_cast<int>(shape.size()), cv::Scalar(255));
}

cv::Range Mask::rowRange(int rowOffset) const {
	return cv::Range(rowOffset, rowOffset + pixels.rows);
}

cv::Range Mask::columnRange(int columnOffset) const {
	return cv::Range(columnOffset, columnOffset + pixels.cols);
}

cv::Mat Mask::copy(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset) {
	cv::Mat result;

	cv::Mat cutout = texture(mask.rowRange(rowOffset), mask.columnRange(columnOffset));
	cutout.copyTo(result, mask.pixels);

	return result;
}

void Mask::cut(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset) {
	cv::Mat cutout(texture, mask.rowRange(rowOffset), mask.columnRange(columnOffset));
	cv::bitwise_and(cutout, cv::Scalar(0), cutout, mask.pixels);
}

void Mask::paste(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset, const Color& color) {
	cv::Mat cutout(texture, mask.rowRange(rowOffset), mask.columnRange(columnOffset));
	cv::Mat(mask.pixels.rows, mask.pixels.cols, CV_8U, cv::Scalar(128)).copyTo(cutout, mask.pixels);
}
