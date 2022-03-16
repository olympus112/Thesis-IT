#include <core.h>
#include "TSPG_Jittered.h"

#include <map>

#include "main.h"

void TSPG_Jittered::renderOverlay(Canvas& source, Canvas& target) {
	if (showGrid)
		for (int i = 0; i < divisions; i++) {
			float y = target.offset.y + target.dimension.y / divisions * i;
			float x = target.offset.x + target.dimension.x / divisions * i;
			ImGui::GetWindowDrawList()->AddLine(ImVec2(target.minX(), y), ImVec2(target.maxX(), y), Colors::WHITE.u32(), 2);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(x, target.minY()), ImVec2(x, target.maxY()), Colors::WHITE.u32(), 2);
		}
}

void TSPG_Jittered::renderSettings(Canvas& source, Canvas& target) {
	std::array methods = { "Random", "Center", "Salience" };

	ImGui::Combo("Jitter method", &jitterMethod, methods.data(), methods.size());
	ImGui::SliderInt("Grid divisions", &divisions, 2, 50);
	ImGui::Checkbox("Show grid", &showGrid);
}

std::vector<MondriaanPatch> TSPG_Jittered::generate() {
	Vec2i targetDimension = settings.target->dimension();
	Vec2i patchDimension = settings.tmm2px(settings.minimumPatchDimension_mm);
	Vec2i safeDimension = Vec2i(targetDimension.x - patchDimension.x, targetDimension.y - patchDimension.y);
	const auto& salience = screen.pipeline.saliencyMap.data;

	std::multimap<double, MondriaanPatch, std::greater<>> patches;

	int xInterval = targetDimension.x / divisions;
	int yInterval = targetDimension.y / divisions;
	//int xInterval = safeDimension.x / divisions;
	//int yInterval = safeDimension.y / divisions;
	for (int i = 0; i < divisions; i++) {
		for (int j = 0; j < divisions; j++) {
			int xMin = i * xInterval;
			int yMin = j * yInterval;
			cv::Rect rect(xMin, yMin, xInterval, yInterval);
			cv::Mat subSalience(salience, rect);

			double value;
			cv::Point relativePoint;
			switch (jitterMethod) {
				case JitterMethod_Random:
					relativePoint = cv::Point(cv::theRNG().uniform(0, xInterval), cv::theRNG().uniform(0, yInterval));
					value = 0;
					break;
				case JitterMethod_Center:
					relativePoint = cv::Point((xInterval - patchDimension.x) / 2, (yInterval - patchDimension.y) / 2);
					value = 0;
					break;
				case JitterMethod_Salience:
					cv::minMaxLoc(subSalience, nullptr, &value, nullptr, &relativePoint);
					break;
			}

			Vec2 targetPosition = Vec2(xMin + relativePoint.x, yMin + relativePoint.y);
			Vec2 sourcePosition = Utils::transform(targetPosition, settings.target->dimension(), settings.source->dimension());
			MondriaanPatch patch(sourcePosition, targetPosition, settings.minimumPatchDimension_mm, 0);

			patches.insert(std::make_pair(value, patch));
		}
	}

	std::vector<MondriaanPatch> result;
	for (const auto& [distance, patch] : patches)
		result.push_back(patch);

	return result;
}