
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

cv::Rect computeRotatedRect(const cv::Size& size, float degrees) {
	return cv::RotatedRect(cv::Point2f(), size, degrees).boundingRect();
}

cv::Mat computeTransformationMatrix(const cv::Size& size, float degrees) {
	const cv::Rect bounds = computeRotatedRect(size, degrees);
	cv::Point2f center(0.5f * static_cast<float>(size.width - 1), 0.5f * static_cast<float>(size.height - 1));

	cv::Mat transformationMatrix = cv::getRotationMatrix2D(center, degrees, 1.0);
	transformationMatrix.at<double>(0, 2) += 0.5 * bounds.width - 0.5 * size.width;
	transformationMatrix.at<double>(1, 2) += 0.5 * bounds.height - 0.5 * size.height;

	return transformationMatrix;
}


int main() {
	float angle = 45;

	cv::Mat alignedMask(50, 50, CV_8UC1, cv::Scalar(255));
	cv::Rect rotatedRect = computeRotatedRect(cv::Size(alignedMask.cols, alignedMask.rows), angle);
	cv::Mat transformation = computeTransformationMatrix(cv::Size(alignedMask.cols, alignedMask.rows), angle);
	cv::Mat rotatedMask;
	cv::warpAffine(alignedMask, rotatedMask, transformation, rotatedRect.size());

	cv::Mat invertedMask;
	cv::bitwise_not(rotatedMask, invertedMask);

	cv::Mat globalMask(500, 500, CV_8UC1, cv::Scalar(255));
	cv::Mat roi = globalMask(cv::Rect(200, 200, invertedMask.cols, invertedMask.rows));
	cv::bitwise_and(roi, invertedMask, roi);

	cv::imshow("Global mask", globalMask);

	cv::boxFilter(globalMask, globalMask, -1, cv::Size(invertedMask.cols, invertedMask.rows), cv::Point(0, 0), true, cv::BORDER_ISOLATED);
	cv::threshold(globalMask, globalMask, 254, 255, cv::THRESH_BINARY);;

	cv::imshow("Filtered global mask", globalMask);

	cv::Mat match;
	cv::matchTemplate(globalMask, rotatedMask, match, cv::TM_SQDIFF_NORMED);
	cv::imshow("Match", match);

	cv::Point point;
	cv::minMaxLoc(match, nullptr, nullptr, &point, nullptr);

	cv::rectangle(globalMask, point, cv::Point(point.x + rotatedMask.cols, point.y + rotatedMask.rows), cv::Scalar(125));

	cv::imshow("Matched global mask", globalMask);

	cv::waitKey();
}
