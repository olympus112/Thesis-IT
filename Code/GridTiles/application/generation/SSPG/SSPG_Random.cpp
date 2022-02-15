#include <core.h>
#include "SSPG_Random.h"

#include <opencv2/imgproc.hpp>

void SSPG_Random::renderSettings(Canvas& source, Canvas& target) {
	ImGui::SliderInt("Interdistance##Source", &interdistance, 10, 500);

}

void SSPG_Random::mutate(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) {
	cv::Mat mask(source.texture->data.rows, source.texture->data.cols, CV_8U, cv::Scalar(255));
	for (int i = 0; i < seedPoints.size(); i++) {
		std::vector<cv::Point> nonZero;
		cv::findNonZero(mask, nonZero);
		if (nonZero.empty())
			return;
		cv::Point point = nonZero[cv::theRNG().uniform(0, nonZero.size())];

		cv::circle(mask, point, interdistance, cv::Scalar(0), -1);

		Vec2 sourcePosition = Vec2(point.x, point.y);
		seedPoints[i].sourcePosition = sourcePosition;
	}
}