#include <core.h>
#include "TSPGeneration.h"

#include "main.h"
#include <map>
#include <opencv2/imgproc.hpp>


void TSPG_RegularGrid::renderOverlay(Canvas& source, Canvas& target) {
	for (int i = 0; i < divisions; i++) {
		float y = target.offset.y + target.dimension.y / divisions * i;
		float x = target.offset.x + target.dimension.x / divisions * i;
		ImGui::GetWindowDrawList()->AddLine(ImVec2(target.minX(), y), ImVec2(target.maxX(), y), Colors::WHITE.u32(), 2);
		ImGui::GetWindowDrawList()->AddLine(ImVec2(x, target.minY()), ImVec2(x, target.maxY()), Colors::WHITE.u32(), 2);
	}
}

void TSPG_RegularGrid::renderSettings(Canvas& source, Canvas& target) {
	ImGui::SliderInt("Grid divisions", &divisions, 2, 50);
}

std::vector<SeedPoint> TSPG_RegularGrid::generate(Canvas& source, Canvas& target) {
	const auto& salience = screen.editor.pipeline.saliencyMap->data;

	std::multimap<double, SeedPoint, std::greater<>> seedPoints;

	for (int i = 0; i < divisions; i++) {
		for (int j = 0; j < divisions; j++) {
			int xInterval = salience.cols / divisions;
			int yInterval = salience.rows / divisions;
			int xMin = i * salience.cols / divisions;
			int yMin = j * salience.rows / divisions;
			cv::Rect rect(xMin, yMin, xInterval, yInterval);
			cv::Mat subTexture(salience, rect);

			double value;
			cv::Point point;
			cv::minMaxLoc(subTexture, nullptr, &value, nullptr, &point);


			seedPoints.insert(std::make_pair(value,
			                                 SeedPoint(
				                                 Utils::transform(Vec2(xMin + point.x, yMin + point.y),
				                                                  target.textureDimension(),
				                                                  source.textureDimension()),
				                                 Vec2(xMin + point.x, yMin + point.y))));
		}
	}

	std::vector<SeedPoint> result;
	for (const auto& [distance, seedPoint] : seedPoints)
		result.push_back(seedPoint);

	return result;
}

void TSPG_Greedy::renderSettings(Canvas& source, Canvas& target) {
	ImGui::SliderInt("# Seedpoints", &count, 1, 50);
	ImGui::SliderInt("Interdistance##Target", &interdistance, 20, 500);
}

std::vector<SeedPoint> TSPG_Greedy::generate(Canvas& source, Canvas& target) {
	const auto& salience = screen.editor.pipeline.saliencyMap->data;

	std::vector<SeedPoint> result;
	cv::Mat mask(salience.rows, salience.cols, CV_8U, cv::Scalar(255));
	for (int i = 0; i < count; i++) {
		cv::Point point;
		cv::minMaxLoc(salience, nullptr, nullptr, nullptr, &point, mask);

		cv::circle(mask, point, interdistance, cv::Scalar(0), -1);

		result.emplace_back(Utils::transform(Vec2(point.x, point.y), target.textureDimension(), source.textureDimension()), Vec2(point.x, point.y));
	}

	return result;
}
