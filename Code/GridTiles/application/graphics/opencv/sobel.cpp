#include "core.h"
#include "sobel.h"

cv::Mat Sobel::computeSobel(const cv::Mat& source, const SobelType& type, int derivative, int size) {
	cv::Mat result;

	if (type == SobelType::MAGNITUDE) {
		cv::Mat floatTexture;
		source.convertTo(floatTexture, CV_32FC1, 1.0 / 65535.0);

		cv::Mat sobelX;
		cv::Mat sobelY;
		cv::Mat sobelMagnitude;

		cv::Sobel(floatTexture, sobelX, CV_32FC1, derivative, 0, size);
		cv::Sobel(floatTexture, sobelY, CV_32FC1, 0, derivative, size);
		cv::magnitude(sobelX, sobelY, sobelMagnitude);

		sobelMagnitude *= 0.25;

		sobelMagnitude.convertTo(sobelMagnitude, CV_8UC1, 65535.0);
		//cv::convertScaleAbs(sobelMagnitude, sobelMagnitude);


		return sobelMagnitude;
	}

	int dx = type == SobelType::X || type == SobelType::XY ? derivative : 0;
	int dy = type == SobelType::Y || type == SobelType::XY ? derivative : 0;
	cv::Sobel(source, result, CV_16U, dx, dy, size);
	cv::convertScaleAbs(result, result);

	return result;
}

Sobel::Sobel(const cv::Mat& texture, const SobelType& type, int derivative, int size) {
	this->type = type;
	this->sobel = computeSobel(texture, type, derivative, size);
}

