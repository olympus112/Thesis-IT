#include <core.h>
#include "SSPG_Sift.h"

#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>

#include "main.h"

static cv::Ptr<cv::SIFT> sift = cv::SIFT::create();

void SSPG_Sift::renderSettings(Canvas& source, Canvas& target) {
	SSPG::renderSettings(source, target);
}

void SSPG_Sift::mutate(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) {
	cv::Mat descriptors;
	std::vector<cv::KeyPoint> keyPoints;
	cv::Mat gray = screen.editor.pipeline.targetGrayscaleE->data;
	sift->detectAndCompute(gray, cv::Mat(), keyPoints, descriptors);
	cv::Mat img;
	cv::drawKeypoints(gray, keyPoints, img, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	cv::imshow("SIFT", img);
}
