#include <core.h>
#include "TSPG_Greedy.h"

#include <opencv2/imgproc.hpp>

#include "main.h"

void TSPG_Greedy::renderSettings(Canvas& source, Canvas& target) {
	const char* methods[] = { "Random", "Salience" };

	ImGui::Combo("Greedy method", &greedyMethod, methods, 2);
	ImGui::SliderInt("# Seedpoints", &count, 1, 50);
	ImGui::SliderInt("Interdistance##Target", &interdistance, 20, 500);
}

std::vector<SeedPoint> TSPG_Greedy::generate(Canvas& source, Canvas& target) {
	const auto& salience = screen.editor.pipeline.saliencyMap->data;

	assert(target.texture->data.rows == salience.rows);
	assert(target.texture->data.cols == salience.cols);

	std::vector<SeedPoint> result;
	cv::Mat mask(target.texture->data.rows, target.texture->data.cols, CV_8U, cv::Scalar(255));
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
		Vec2 sourcePosition = Utils::transform(targetPosition, target, source);
		result.emplace_back(sourcePosition, targetPosition);
	}

	return result;
}
