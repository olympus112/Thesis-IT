#include "core.h"
#include "mask.h"

#include <opencv2/imgproc.hpp>

Mask::Mask(const Shape& shape) {
	Vec2i dimension = shape.dimension;

	std::vector<cv::Point> points(shape.size());
	for (std::size_t index = 0; index < shape.size(); index++) 
		points[index] = shape[index].cv();
	
	this->data = cv::Mat(dimension.y, dimension.x, CV_8U, cv::Scalar(0));

	cv::fillConvexPoly(this->data, points.data(), static_cast<int>(shape.size()), cv::Scalar(255));
}

Mask::Mask(const Vec2& dimension, double rotation) {
	this->data = Mask::rotated(dimension, rotation);
}

cv::Range Mask::rowRange(int rowOffset) const {
	return cv::Range(rowOffset, rowOffset + data.rows);
}

cv::Range Mask::columnRange(int columnOffset) const {
	return cv::Range(columnOffset, columnOffset + data.cols);
}

cv::Mat Mask::copy(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset) {
	cv::Mat result;

	cv::Mat cutout = texture(mask.rowRange(rowOffset), mask.columnRange(columnOffset));
	cutout.copyTo(result, mask.data);

	return result;
}

void Mask::cut(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset) {
	cv::Mat cutout(texture, mask.rowRange(rowOffset), mask.columnRange(columnOffset));
	cv::bitwise_and(cutout, cv::Scalar(0), cutout, mask.data);
}
 
void Mask::paste(const cv::Mat& texture, const Mask& mask, int rowOffset, int columnOffset, const Color& color) {
	cv::Mat cutout(texture, mask.rowRange(rowOffset), mask.columnRange(columnOffset));
	cutout.setTo(color.cv(), mask.data);
}

cv::Mat Mask::fromPoints(const Bounds& bounds, const std::vector<Vec2>& points) {
	cv::Mat result(cv::Size(bounds.width(), bounds.height()), CV_8UC1);
	cv::Point vertices[4] { points[0].cv(), points[1].cv(), points[2].cv() };

	cv::fillConvexPoly(result, vertices, 4, cv::Scalar(255));

	return result;
}

cv::Mat Mask::rotated(const Vec2& dimension, double rotation) {

	cv::Point center(dimension.cv() / 2.0);
	cv::Size size(dimension.cv());

	cv::RotatedRect rotatedRect(center, size, static_cast<float>(rotation));
	cv::Rect bounds = rotatedRect.boundingRect();
	rotatedRect.center += cv::Point2f(cv::abs(bounds.x), cv::abs(bounds.y));

	cv::Mat result(bounds.size(), CV_8UC1, cv::Scalar(0));

	cv::Point2f points[4];
	rotatedRect.points(points);

	cv::Point vertices[4] { points[0], points[1], points[2], points[3] };
	cv::fillConvexPoly(result, vertices, 4, cv::Scalar(255));

	return result;
}
