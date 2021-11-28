#include "core.h"
#include "blur.h"

cv::Mat Blur::computeBlur(const cv::Mat& source, const cv::Size& size, const Vec2& sigma) {
	cv::Mat result;

	cv::GaussianBlur(source, result, size, sigma.x, sigma.y);

	return result;
}

Blur::Blur(const cv::Mat& texture, const cv::Size& size, const Vec2& sigma) {
	this->size = size;
	this->sigma = sigma;
	this->blur = computeBlur(texture, size, sigma);
}