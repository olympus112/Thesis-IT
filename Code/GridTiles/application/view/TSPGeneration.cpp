#include <core.h>
#include "TSPGeneration.h"

#include "main.h"
#include <map>
#include <opencv2/imgproc.hpp>

std::unordered_map<TSPGenerationMethod, SRef<TSPG_Method>> tspGenerationMethods = {
	{TSPGenerationMethod_Jittered, std::make_shared<TSPG_Jittered>()},
	{TSPGenerationMethod_Greedy, std::make_shared<TSPG_Greedy>()}
};

//! -------------------------------------------------------------------------------------------------------

void TSPG_Jittered::renderOverlay(Canvas& source, Canvas& target) {
	for (int i = 0; i < divisions; i++) {
		float y = target.offset.y + target.dimension.y / divisions * i;
		float x = target.offset.x + target.dimension.x / divisions * i;
		ImGui::GetWindowDrawList()->AddLine(ImVec2(target.minX(), y), ImVec2(target.maxX(), y), Colors::WHITE.u32(), 2);
		ImGui::GetWindowDrawList()->AddLine(ImVec2(x, target.minY()), ImVec2(x, target.maxY()), Colors::WHITE.u32(), 2);
	}
}

void TSPG_Jittered::renderSettings(Canvas& source, Canvas& target) {
	const char* methods[] = {"Random", "Center", "Salience"};

	ImGui::Combo("Jitter method", &jitterMethod, methods, 3);
	ImGui::SliderInt("Grid divisions", &divisions, 2, 50);
	ImGui::Checkbox("Show grid", &showGrid);
}

std::vector<SeedPoint> TSPG_Jittered::generate(Canvas& source, Canvas& target) {
	const auto& salience = screen.editor.pipeline.saliencyMap->data;

	std::multimap<double, SeedPoint, std::greater<>> seedPoints;

	for (int i = 0; i < divisions; i++) {
		for (int j = 0; j < divisions; j++) {
			int xInterval = target.texture->data.cols / divisions;
			int yInterval = target.texture->data.rows / divisions;
			int xMin = i * target.texture->data.cols / divisions;
			int yMin = j * target.texture->data.rows / divisions;
			cv::Rect rect(xMin, yMin, xInterval, yInterval);
			cv::Mat subSalience(salience, rect);

			double value;
			cv::Point relativePoint;
			switch (jitterMethod) {
				case JitterMethod_Random:
					relativePoint = cv::Point( cv::theRNG().uniform(0, xInterval), cv::theRNG().uniform(0, yInterval));
					value = 0;
					break;
				case JitterMethod_Center:
					relativePoint = cv::Point(xInterval / 2,  yInterval / 2);
					value = 0;
					break;
				case JitterMethod_Salience:
					cv::minMaxLoc(subSalience, nullptr, &value, nullptr, &relativePoint);
					break;
			}

			Vec2 targetPosition = Vec2(xMin + relativePoint.x, yMin + relativePoint.y);
			Vec2 sourcePosition = Utils::transform(targetPosition, target, source);
			SeedPoint seedPoint(sourcePosition, targetPosition);

			seedPoints.insert(std::make_pair(value, seedPoint));
		}
	}

	std::vector<SeedPoint> result;
	for (const auto& [distance, seedPoint] : seedPoints)
		result.push_back(seedPoint);

	return result;
}

//! -------------------------------------------------------------------------------------------------------

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
