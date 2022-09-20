#include <core.h>
#include "TSPG_Jittered.h"

#include <map>

#include "main.h"

void TSPG_Jittered::renderOverlay(Canvas& source, Canvas& target) {
	if (showGrid) {
		for (int i = 0; i < divisions; i++) {
			float y = target.offset.y + target.dimension.y / divisions * i;
			float x = target.offset.x + target.dimension.x / divisions * i;
			ImGui::GetWindowDrawList()->AddLine(ImVec2(target.minX(), y), ImVec2(target.maxX(), y), Colors::WHITE.u32(), 2);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(x, target.minY()), ImVec2(x, target.maxY()), Colors::WHITE.u32(), 2);
		}
	}
}

void TSPG_Jittered::renderSettings(Canvas& source, Canvas& target) {
	std::array methods = { "Random", "Center", "Salience", "Start"};

	ImGui::Combo("Jitter method", &jitterMethod, methods.data(), methods.size());
	ImGui::SliderInt("Grid divisions", &divisions, 2, 50);
	ImGui::Checkbox("Show grid##JitterShowGrid", &showGrid);
}

std::vector<MondriaanPatch> TSPG_Jittered::generate() {
	Vec2 targetDimension = settings.target->dimension();
	Vec2 patchDimension = settings.tmm2px(settings.minimumPatchDimension_mm);
	Vec2 safeDimension = Vec2(targetDimension.x - patchDimension.x, targetDimension.y - patchDimension.y);
	const auto& salience = screen.pipeline.saliencyMap.data;

	std::multimap<double, MondriaanPatch, std::greater<>> patches;

	double xInterval = targetDimension.x / divisions;
	double yInterval = targetDimension.y / divisions;

	for (int i = 0; i < divisions; i++) {
		for (int j = 0; j < divisions; j++) {
			int xMin = i * xInterval;
			int yMin = j * yInterval;
			cv::Rect rect(xMin, yMin, xInterval - (contain ? patchDimension.x : 0), yInterval - (contain ? patchDimension.y : 0));
			cv::Mat subSalience(salience, rect);

			double value;
			cv::Point relativePoint;
			switch (jitterMethod) {
				case JitterMethod_Random:
					relativePoint = cv::Point(cv::theRNG().uniform(0, static_cast<int>(xInterval)), cv::theRNG().uniform(0, static_cast<int>(yInterval)));
					value = 0;
					break;
				case JitterMethod_Center:
					relativePoint = cv::Point(static_cast<int>((xInterval - patchDimension.x) / 2.0), static_cast<int>((yInterval - patchDimension.y) / 2.0));
					value = 0;
					break;
				case JitterMethod_Salience:
					cv::minMaxLoc(subSalience, nullptr, &value, nullptr, &relativePoint);
					break;
				case JitterMethod_Start:
					relativePoint = cv::Point(0, 0);
					value = 0;
					break;
			}

			Vec2 targetPosition = Vec2(xMin + relativePoint.x, yMin + relativePoint.y);
			Vec2 sourcePosition = Utils::transform(targetPosition, settings.target->dimension(), settings.source->dimension());
			MondriaanPatch patch(sourcePosition, targetPosition, settings.minimumPatchDimension_mm);
			/*patch.computeTransformationMatrix();
			patch.computeMask();*/

			patches.insert(std::make_pair(value, patch));
		}
	}

	std::vector<MondriaanPatch> result;
	for (const auto& [distance, patch] : patches)
		result.push_back(patch);

	return result;
}