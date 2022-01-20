#include "core.h"
#include "grayscale.h"

cv::Mat Grayscale::computeGrayscale(const cv::Mat& source) {
	cv::Mat result;
	cv::cvtColor(source, result, cv::COLOR_BGR2GRAY);

	return result;
}

Grayscale::Grayscale(Texture* texture) : Grayscale(texture->data) {}

Grayscale::Grayscale(const cv::Mat& texture) {
	this->grayscale = computeGrayscale(texture);
}
