#include "core.h"
#include "canny.h"
cv::Mat Canny::computeCanny(const cv::Mat& source, double threshold1, double threshold2,
                            int apertureSize, bool L2gradient) {
	cv::Mat result;

	cv::Canny(source, result, threshold1, threshold2, apertureSize, L2gradient);

	return result;
}

Canny::Canny(const cv::Mat& texture, double threshold1, double threshold2, int apertureSize, bool L2gradient) {
	this->threshold1 = threshold1;
	this->threshold2 = threshold2;
	this->apertureSize = apertureSize;
	this->L2gradient = L2gradient;
	this->canny = computeCanny(texture, threshold1, threshold2, apertureSize, L2gradient);
}
