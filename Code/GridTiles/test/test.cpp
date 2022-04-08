
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "math/utils.h"

int main() {
	cv::Mat image(10, 10, CV_8UC1, cv::Scalar(255));
	cv::Mat templ(3, 3, CV_8UC1, cv::Scalar(0));

	cv::Mat result;
	Utils::matchTemplate(image, templ, result, 0, cv::Mat(), cv::Mat());

	cv::imshow("result", result);
	cv::waitKey();
}
