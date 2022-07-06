#include <core.h>
#include "SSPG_Random.h"

#include <opencv2/imgproc.hpp>

#include "main.h"

void SSPG_Random::renderSettings(Canvas& source, Canvas& target) {
	ImGui::SliderInt("Interdistance##Source", &interdistance, 10, 500);

}

void SSPG_Random::mutate(std::vector<MondriaanPatch>& patches) {
	cv::Mat mask(settings.sourcer->dimension().cv(), CV_8U, cv::Scalar(255));

	for (MondriaanPatch& patch : patches) {
		std::vector<cv::Point> nonZero;
		cv::findNonZero(mask, nonZero);
		if (nonZero.empty())
			return;
		cv::Point point = nonZero[cv::theRNG().uniform(0, nonZero.size())];

		cv::circle(mask, point, interdistance, cv::Scalar(0), -1);

		Vec2 sourcePosition = Vec2(point.x, point.y);
		patch.sourceOffset = sourcePosition;
	}
}