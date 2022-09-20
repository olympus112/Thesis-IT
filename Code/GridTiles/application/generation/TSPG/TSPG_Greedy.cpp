#include <core.h>
#include "TSPG_Greedy.h"

#include <opencv2/imgproc.hpp>

#include "main.h"

void TSPG_Greedy::renderSettings(Canvas& source, Canvas& target) {
	std::array methods = { "Random", "Salience" };

	ImGui::Combo("Greedy method", &greedyMethod, methods.data(), methods.size());
	ImGui::SliderInt("# Seedpoints", &count, 1, 50);
	ImGui::SliderInt("Interdistance##Target", &interdistance, 20, 500);
}

std::vector<MondriaanPatch> TSPG_Greedy::generate() {
	assert(settings.target->cols() == screen.pipeline.saliencyMap.cols());
	assert(settings.target->rows() == screen.pipeline.saliencyMap.rows());

	Vec2i targetDimension = settings.target->dimension();
	Vec2i patchDimension = settings.tmm2px(settings.minimumPatchDimension_mm);
	Vec2i safeDimension = Vec2i(targetDimension.x - patchDimension.x, targetDimension.y - patchDimension.y);
	cv::Rect safeRegion = cv::Rect(0, 0, safeDimension.x, safeDimension.y);
	const auto& salience = screen.pipeline.saliencyMap.data(safeRegion);

	std::vector<MondriaanPatch> result;
	cv::Mat mask(safeDimension.cv(), CV_8U, cv::Scalar(255));
	for (int i = 0; i < count; i++) {
		cv::Point point;
		switch (greedyMethod) {
			case GreedyMethod_Random: {
				std::vector<cv::Point> nonZero;
				cv::findNonZero(mask, nonZero);
				if (nonZero.empty())
					return result;

				point = nonZero[cv::theRNG().uniform(0, nonZero.size())];
				break;
			} case GreedyMethod_Salience:
				cv::minMaxLoc(salience, nullptr, nullptr, nullptr, &point, mask);
				break;
		}

		cv::circle(mask, point, interdistance, cv::Scalar(0), -1);

		Vec2 targetPosition = Vec2(point.x, point.y);
		Vec2 sourcePosition = Utils::transform(targetPosition, settings.target->dimension(), settings.source->dimension());

		result.emplace_back(sourcePosition, targetPosition, settings.minimumPatchDimension_mm);
	}

	return result;
}
