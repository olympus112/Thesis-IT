#include <core.h>
#include "seedpoints.h"
#include <format>
#include <algorithm>
#include <map>
#include <queue>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "main.h"
#include "screen.h"
#include "graphics/color.h"
#include "graphics/imgui/widgets.h"
#include "graphics/imgui/imguiUtils.h"
#include "omp.h"
#include "graphics/mondriaanPatch.h"

//! -------------
//! SeedPointsTab
//! -------------

void SeedPointsTab::init() {
	generator = std::mt19937(std::random_device()());
}

void SeedPointsTab::update() {
	ImVec2 relativeOffset = ImGui::GetMousePos() - (source.hover ? source : target).offset;

	// Mouse move
	if (source.hover || target.hover) {
		intersectedIndex = -1;
		intersectedPoint = relativeOffset;

		for (int index = 0; index < seedPoints.size(); index++) {
			const auto& [sourcePosition, targetPosition, textureSize, patch] = seedPoints[index];

			if (source.hover && Bounds(sourcePosition, textureSize).contains(
				source.toTextureSpace(intersectedPoint))) {
				intersectedIndex = index;
				break;
			}

			if (target.hover && Bounds(targetPosition, textureSize).contains(
				target.toTextureSpace(intersectedPoint))) {
				intersectedIndex = index;
				break;
			}
		}
	} else {
		source.drag = false;
		target.drag = false;
	}

	// Mouse press
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		if (source.hover || target.hover) {
			selectedPoint = intersectedPoint;
			selectedIndex = intersectedIndex;

			if (target.hover)
				target.drag = true;

			if (source.hover)
				source.drag = true;
		}
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && (source.hover || target.hover)) {
		seedPoints.emplace_back(source.toTextureSpace(relativeOffset),
		                        target.toTextureSpace(relativeOffset));
	}

	// Mouse drag
	if ((source.drag || target.drag) && selectedIndex != -1) {
		Vec2 delta = relativeOffset - selectedPoint;
		if (source.drag)
			seedPoints[selectedIndex].sourcePosition += source.toTextureSpace(delta);

		if (target.drag)
			seedPoints[selectedIndex].targetPosition += target.toTextureSpace(delta);

		selectedPoint += delta;
	}

	// Mouse release
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		target.drag = false;
		source.drag = false;
	}
}

void SeedPointsTab::renderCursor() {
	// Cursor
	if (source.hover || target.hover) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		double size = 7;
		float thickness = 3;
		Vec2 cursor = ImGui::GetMousePos();
		ImGui::GetWindowDrawList()->AddLine(cursor - ImVec2(static_cast<float>(size), 0),
		                                    cursor + ImVec2(static_cast<float>(size), 0), Colors::WHITE.u32(),
		                                    thickness);
		ImGui::GetWindowDrawList()->AddLine(cursor - ImVec2(0, static_cast<float>(size)),
		                                    cursor + ImVec2(0, static_cast<float>(size)), Colors::WHITE.u32(),
		                                    thickness);

		if (source.hover) {
			ImGui::GetWindowDrawList()->AddLine(Vec2(source.minX(), cursor.y).iv(),
			                                    Vec2(source.minX() + size, cursor.y).iv(), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(source.maxX(), cursor.y).iv(),
			                                    Vec2(source.maxX() - size, cursor.y).iv(), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(cursor.x, source.minY()).iv(),
			                                    Vec2(cursor.x, source.minY() + size).iv(), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(cursor.x, source.maxY()).iv(),
			                                    Vec2(cursor.x, source.maxY() - size).iv(), Colors::WHITE.u32(),
			                                    thickness);
		}

		if (target.hover) {
			ImGui::GetWindowDrawList()->AddLine(Vec2(target.minX(), cursor.y).iv(),
			                                    Vec2(target.minX() + size, cursor.y).iv(), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(target.maxX(), cursor.y).iv(),
			                                    Vec2(target.maxX() - size, cursor.y).iv(), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(cursor.x, target.minY()).iv(),
			                                    Vec2(cursor.x, target.minY() + size).iv(), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(Vec2(cursor.x, target.maxY()).iv(),
			                                    Vec2(cursor.x, target.maxY() - size).iv(), Colors::WHITE.u32(),
			                                    thickness);
		}
	}
}

void SeedPointsTab::renderTooltip() {
	// Tooltip
	if (intersectedIndex != -1 || selectedIndex != -1) {
		const auto& seedPoint = seedPoints[intersectedIndex != -1 ? intersectedIndex : selectedIndex];
		auto sourceScreenPosition = source.toAbsoluteScreenSpace(seedPoint.sourcePosition);
		auto targetScreenPosition = target.toAbsoluteScreenSpace(seedPoint.targetPosition);
		auto tooltipPosition = intersectedIndex != -1
			                       ? ImGui::GetCursorPos()
			                       : source.min() + seedPoint.sourcePosition;

		Bounds sourcePatch(sourceScreenPosition, 20);
		Bounds targetPatch(targetScreenPosition, 20);
		Bounds sourceUV = source.uv(sourcePatch);
		Bounds targetUV = target.uv(targetPatch);

		ImVec2 restore = ImGui::GetCursorPos();

		ImGui::SetCursorPos(tooltipPosition.iv());
		ImGui::SetNextWindowSize(ImVec2(190, 0));
		ImGui::BeginTooltip();

		ImGui::TextColored(Colors::BLUE.iv4(), "General");
		ImGui::Separator();
		ImGui::Text("Patch size: %.2f", 20.0f);
		ImGui::Separator();

		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, 95);
		ImGui::TextColored(Colors::BLUE.iv4(), "Source");
		ImGui::Text("X: %.0f", seedPoint.sourcePosition.x);
		ImGui::Text("Y: %.0f", seedPoint.sourcePosition.y);
		ImGui::image("Source", source.texture->it(), ImVec2(80, 80),
		             sourceUV.min().iv(), sourceUV.max().iv());

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, 95);
		ImGui::TextColored(Colors::BLUE.iv4(), "Target");
		ImGui::Text("X: %.0f", seedPoint.targetPosition.x);
		ImGui::Text("Y: %.0f", seedPoint.targetPosition.y);
		ImGui::image("Target", target.texture->it(), ImVec2(80, 80),
		             targetUV.min().iv(),
		             targetUV.max().iv());

		ImGui::Columns(1);

		ImGui::EndTooltip();

		ImGui::SetCursorPos(restore);
	}
}

void SeedPointsTab::renderSeedPointViewer() {
	// Seedpoint viewer
	ImGui::Begin("Seedpoint viewer");
	for (int index = 0; index < seedPoints.size(); index++) {
		const auto& seedPoint = seedPoints[index];
		auto sourceScreenPosition = source.toAbsoluteScreenSpace(seedPoint.sourcePosition);
		auto targetScreenPosition = target.toAbsoluteScreenSpace(seedPoint.targetPosition);
		Bounds sourcePatch(sourceScreenPosition, 20.0);
		Bounds targetPatch(targetScreenPosition, 20.0);
		Bounds sourceUV = source.uv(sourcePatch);
		Bounds targetUV = target.uv(targetPatch);

		float size = 80.0f;
		ImGui::TextColored(Colors::BLUE.iv4(), "Seedpoint #%d", index + 1);

		ImGui::Columns(4, 0, false);
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Source");
		ImGui::Image(source.texture->it(), ImVec2(size, size),
		             sourceUV.min().iv(),
		             sourceUV.max().iv());
		ImGui::NextColumn();
		ImGui::Text("");
		ImGui::Text("X: %.0f", seedPoint.sourcePosition.x);
		ImGui::Text("Y: %.0f", seedPoint.sourcePosition.y);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Target");
		ImGui::Image(target.texture->it(), ImVec2(size, size),
		             targetUV.min().iv(),
		             targetUV.max().iv());
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::Text("");
		ImGui::Text("X: %.0f", seedPoint.targetPosition.x);
		ImGui::Text("Y: %.0f", seedPoint.targetPosition.y);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::Image(source.features[Canvas::FEATURE_INT]->it(), ImVec2(size, size),
		             sourceUV.min().iv(),
		             sourceUV.max().iv());
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::Image(source.features[Canvas::FEATURE_SOBEL]->it(), ImVec2(size, size),
		             sourceUV.min().iv(),
		             sourceUV.max().iv());
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::Image(target.features[Canvas::FEATURE_INT]->it(), ImVec2(size, size),
		             targetUV.min().iv(),
		             targetUV.max().iv());
		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::Image(target.features[Canvas::FEATURE_SOBEL]->it(), ImVec2(size, size),
		             targetUV.min().iv(),
		             targetUV.max().iv());
		ImGui::NextColumn();

		ImGui::Columns(1);

		ImGui::Separator();
	}
	ImGui::End();
}

void SeedPointsTab::renderPatchViewer() {
	// Patch viewer
	ImGui::Begin("Patch viewer");
	for (int index = 0; index < seedPoints.size(); index++) {
		Patch* patch = seedPoints[index].patch.get();
		if (patch == nullptr)
			continue;

		auto sourceScreenPosition = source.toAbsoluteScreenSpace(patch->sourceOffset);
		auto targetScreenPosition = target.toAbsoluteScreenSpace(patch->targetOffset);
		Bounds sourcePatch(sourceScreenPosition, patch->dimension().x, patch->dimension().y);
		Bounds targetPatch(targetScreenPosition, patch->dimension().x, patch->dimension().y);
		Bounds sourceUV = source.uv(sourcePatch);
		Bounds targetUV = target.uv(targetPatch);

		float size = 80.0f;
		ImGui::TextColored(Colors::BLUE.iv4(), "Patch #%d", index + 1);

		ImGui::Columns(4, 0, false);
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Source");
		ImGui::Image(source.texture->it(), ImVec2(size, size),
		             sourceUV.min().iv(),
		             sourceUV.max().iv());
		patch->render(Bounds(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()), false, Colors::RGB_R);

		ImGui::NextColumn();
		ImGui::Text("");
		ImGui::Text("X: %.0f", patch->sourceOffset.x);
		ImGui::Text("Y: %.0f", patch->sourceOffset.y);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Target");
		ImGui::Image(target.texture->it(), ImVec2(size, size),
		             targetUV.min().iv(),
		             targetUV.max().iv());
		patch->render(Bounds(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()), false, Colors::RGB_R);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);

		ImGui::Text("");
		ImGui::Text("X: %.0f", patch->targetOffset.x);
		ImGui::Text("Y: %.0f", patch->targetOffset.y);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);

		ImGui::Image(source.features[Canvas::FEATURE_INT]->it(), ImVec2(size, size),
		             sourceUV.min().iv(),
		             sourceUV.max().iv());
		patch->render(Bounds(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()), false, Colors::RGB_R);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);

		ImGui::Image(source.features[Canvas::FEATURE_SOBEL]->it(), ImVec2(size, size),
		             sourceUV.min().iv(),
		             sourceUV.max().iv());
		patch->render(Bounds(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()), false, Colors::RGB_R);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);

		ImGui::Image(target.features[Canvas::FEATURE_INT]->it(), ImVec2(size, size),
		             targetUV.min().iv(),
		             targetUV.max().iv());
		patch->render(Bounds(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()), false, Colors::RGB_R);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);

		ImGui::Image(target.features[Canvas::FEATURE_SOBEL]->it(), ImVec2(size, size),
		             targetUV.min().iv(),
		             targetUV.max().iv());
		patch->render(Bounds(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()), false, Colors::RGB_R);

		ImGui::NextColumn();

		ImGui::Columns(1);

		ImGui::Text("Dimension: (%.0f, %.0f)", patch->dimension().x, patch->dimension().y);

		ImGui::Separator();
	}
	ImGui::End();
}

void SeedPointsTab::renderPatches() {
	// Render patches
	for (int index = 0; index < seedPoints.size(); index++) {
		if (Patch* patch = seedPoints[index].patch.get())
			patch->render(source, target);
	}
}

void SeedPointsTab::renderSeedPoints() {
	// Render seedpoints
	for (int index = 0; index < seedPoints.size(); index++)
		seedPoints[index].render(source, target, intersectedIndex == index, selectedIndex == index, showConnections,
		                         Color::blend(Colors::YELLOW, Colors::RGB_R,
		                                      static_cast<float>(index + 1) / static_cast<float>(seedPoints.size())));
}

void SeedPointsTab::renderTextures() {
	ImGui::Columns(3, nullptr, false);
	ImGui::SetColumnWidth(0, source.dimension.iv().x + 10);
	ImGui::SetColumnWidth(1, target.dimension.iv().x + 10);

	// Source image
	ImGui::image("Source", source.texture->it(), source.dimension.iv());
	source.offset = ImGui::GetItemRectMin();
	source.hover = ImGui::IsItemHovered();

	ImGui::NextColumn();

	// Target image
	ImGui::image("Target", target.texture->it(), target.dimension.iv());
	target.offset = ImGui::GetItemRectMin();
	target.hover = ImGui::IsItemHovered();

	ImGui::NextColumn();

	// Target
	ImGui::TextColored(Colors::BLUE.iv4(), "Target seedpoints");
	ImGui::Separator();
	ImGui::Checkbox("Grid method", &targetSeedPointsGridMethod);
	bool showGrid = false;
	if (!targetSeedPointsGridMethod) {
		ImGui::SliderInt("# Seedpoints", &targetSeedPoints, 1, 50);
		ImGui::SliderInt("Interdistance", &targetSeedPointInterDistance, 20, 500);
	} else {
		ImGui::SliderInt("Grid size", &targetSeedPointsGridDivisions, 2, 50);
		showGrid = ImGui::IsItemActive();
	}

	// Source
	ImGui::TextColored(Colors::BLUE.iv4(), "Source seedpoints");
	ImGui::Separator();
	ImGui::SliderInt("Interdistance", &sourceSeedPointInterDistance, 20, 500);
	ImGui::RadioButton("SQDIFF", &sourceSeedPointsMethod, cv::TM_SQDIFF);
	ImGui::SameLine();
	ImGui::RadioButton("SQDIFF_NORMED", &sourceSeedPointsMethod, cv::TM_SQDIFF_NORMED);
	ImGui::RadioButton("CCOEFF", &sourceSeedPointsMethod, cv::TM_CCOEFF);
	ImGui::SameLine();
	ImGui::RadioButton("CCOEFF_NORMED", &sourceSeedPointsMethod, cv::TM_CCOEFF_NORMED);
	ImGui::RadioButton("CCORR", &sourceSeedPointsMethod, cv::TM_CCORR);
	ImGui::SameLine();
	ImGui::RadioButton("TCCORR_NORMED", &sourceSeedPointsMethod, cv::TM_CCORR_NORMED);

	ImGui::NextColumn();
	ImGui::Columns(1);

	if (showGrid)
		renderTargetGrid();

	// Bounding boxes
	ImGui::GetWindowDrawList()->AddRect(source.min().iv(), source.max().iv(), Colors::WHITE.u32(), 0,
	                                    ImDrawCornerFlags_All,
	                                    2);
	ImGui::GetWindowDrawList()->AddRect(target.min().iv(), target.max().iv(), Colors::WHITE.u32(), 0,
	                                    ImDrawCornerFlags_All,
	                                    2);
}

void SeedPointsTab::renderTargetGrid() {
	for (int i = 0; i < targetSeedPointsGridDivisions; i++) {
		float y = target.offset.y + target.dimension.y / targetSeedPointsGridDivisions * i;
		float x = target.offset.x + target.dimension.x / targetSeedPointsGridDivisions * i;
		ImGui::GetWindowDrawList()->AddLine(
			ImVec2(target.minX(), y),
			ImVec2(target.maxX(), y),
			Colors::WHITE.u32(),
			2
		);
		ImGui::GetWindowDrawList()->AddLine(
			ImVec2(x, target.minY()),
			ImVec2(x, target.maxY()),
			Colors::WHITE.u32(),
			2
		);
	}
}

void SeedPointsTab::renderSettings() {
	float height = 30.0f;

	ImGuiUtils::pushButtonColor(0.0);
	ImGui::SetCursorPosX(static_cast<float>(source.minX() - ImGui::GetWindowPos().x));
	if (ImGui::Button("Delete seedpoints", ImVec2(source.dimension.x, height)))
		seedPoints.clear();
	ImGui::SameLine();
	ImGui::SetCursorPosX(static_cast<float>(target.minX() - ImGui::GetWindowPos().x));
	if (ImGui::Button("Delete selected seedpoint", ImVec2(target.dimension.x, height)))
		if (selectedIndex != -1)
			seedPoints.erase(seedPoints.begin() + selectedIndex);
	ImGuiUtils::popButtonColor();

	ImGuiUtils::pushButtonColor(0.28);
	ImGui::SetCursorPosX(static_cast<float>(source.minX() - ImGui::GetWindowPos().x));
	if (ImGui::Button("Move source seedpoints", ImVec2(source.dimension.x, height)))
		spawnSourceSeedpoints();
	ImGui::SameLine();
	ImGui::SetCursorPosX(static_cast<float>(target.minX() - ImGui::GetWindowPos().x));
	if (ImGui::Button("Spawn target seedpoints", ImVec2(target.dimension.x, height)))
		spawnTargetSeedpoints();
	ImGuiUtils::popButtonColor();

	ImGui::SetCursorPosX(static_cast<float>(source.minX() - ImGui::GetWindowPos().x));
	if (ImGui::Button("Spawn patches", ImVec2(source.dimension.x, height)))
		spawnPatches();
	ImGui::SameLine();
	ImGui::SetCursorPosX(static_cast<float>(target.minX() - ImGui::GetWindowPos().x));
	if (ImGui::Button("Mutate patches", ImVec2(target.dimension.x, height)))
		mutatePatches();

	ImGui::Checkbox("Show connections", &showConnections);
	if (ImGui::Button("Generate image"))
		generateImage();

	if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsItemHovered())
		mutatePatches();
}

void SeedPointsTab::render() {
	ImGui::Begin("SeedPoints", 0, ImGuiWindowFlags_AlwaysAutoResize);

	renderTextures();
	renderSeedPoints();
	renderPatches();
	renderSettings();
	renderCursor();
	renderTooltip();

	ImGui::End();

	renderPatchViewer();
	renderSeedPointViewer();
}

void SeedPointsTab::spawnTargetSeedpoints() {
	const auto& salience = screen->editor->pipelineTab->saliencyMap->data;
	seedPoints.clear();
	resetSelection();

	if (targetSeedPointsGridMethod) {
		std::multimap<double, SeedPoint, std::greater<>> seedPoints;

		for (int i = 0; i < targetSeedPointsGridDivisions; i++) {
			for (int j = 0; j < targetSeedPointsGridDivisions; j++) {
				int xInterval = salience.cols / targetSeedPointsGridDivisions;
				int yInterval = salience.rows / targetSeedPointsGridDivisions;
				int xMin = i * salience.cols / targetSeedPointsGridDivisions;
				int yMin = j * salience.rows / targetSeedPointsGridDivisions;
				cv::Rect rect(xMin, yMin, xInterval, yInterval);
				cv::Mat subTexture(salience, rect);

				double value;
				cv::Point point;
				cv::minMaxLoc(subTexture, nullptr, &value, nullptr, &point);


				seedPoints.insert(
					std::make_pair(
						value,
						SeedPoint(
							Utils::transform(Vec2(xMin + point.x, yMin + point.y),
							                 target.textureDimension(),
							                 source.textureDimension()),
							Vec2(xMin + point.x, yMin + point.y)
						)
					)
				);
			}
		}

		for (const auto& [distance, seedPoint] : seedPoints)
			this->seedPoints.push_back(seedPoint);

	} else {
		cv::Mat mask(salience.rows, salience.cols, CV_8U, cv::Scalar(255));
		for (int i = 0; i < targetSeedPoints; i++) {
			cv::Point point;
			cv::minMaxLoc(salience, nullptr, nullptr, nullptr, &point, mask);

			cv::circle(mask, point, targetSeedPointInterDistance, cv::Scalar(0), -1);

			this->seedPoints.emplace_back(
				Utils::transform(Vec2(point.x, point.y), target.textureDimension(),
				                 source.textureDimension()),
				Vec2(point.x, point.y)
			);
		}
	}
}

void SeedPointsTab::spawnSourceSeedpoints() {
	std::vector<double> distribution = {screen->settings->intensityWeight, screen->settings->edgeWeight};

	int textureRows = source.texture->data.rows;
	int textureCols = source.texture->data.cols;
	cv::Mat mask(textureRows, textureCols, CV_8UC1, cv::Scalar(255));

	for (auto& seedPoint : seedPoints) {
		cv::Rect rect = seedPoint.targetBounds(target.texture).cv();
		int patchRows = rect.height;
		int patchCols = rect.width;

		// Create all features
		std::vector<cv::Mat> features(source.features.size());
#pragma omp parallel for
		for (int featureIndex = 0; featureIndex < source.features.size(); featureIndex++) {
			cv::Mat patch(target.features[featureIndex]->data, rect);

			cv::matchTemplate(source.features[featureIndex]->data, patch, features[featureIndex],
			                  sourceSeedPointsMethod);
			// Todo: check need
			cv::normalize(features[featureIndex], features[featureIndex], 1.0, 0.0, cv::NORM_MINMAX);
		}

		// Merge features
		int featureRows = textureRows - patchRows + 1;
		int featureCols = textureCols - patchCols + 1;
		cv::Mat output(featureRows, featureCols, CV_32F, cv::Scalar(0.0));
		for (int i = 0; i < features.size(); i++)
			cv::addWeighted(output, 1.0, features[i], distribution[i], 0.0, output);

		// Find brightest point
		cv::Point point;
		if (sourceSeedPointsMethod == cv::TM_SQDIFF || sourceSeedPointsMethod == cv::TM_SQDIFF_NORMED)
			cv::minMaxLoc(output, nullptr, nullptr, &point, nullptr, mask(cv::Rect(0, 0, featureCols, featureRows)));
		else
			cv::minMaxLoc(output, nullptr, nullptr, nullptr, &point, mask(cv::Rect(0, 0, featureCols, featureRows)));

		// Correct point
		cv::Point center(point.x + patchCols / 2, point.y + patchRows / 2);

		// Set mask
		cv::circle(mask, center, sourceSeedPointInterDistance, cv::Scalar(0), -1);

		// Move seedpoint
		seedPoint.sourcePosition = Vec2(center.x, center.y);
	}
}

void SeedPointsTab::spawnPatches() {
	for (auto& seedPoint : seedPoints) {
		Vec2 sourceCenter(seedPoint.sourcePosition.x - seedPoint.textureSize / 2,
		                  seedPoint.sourcePosition.x - seedPoint.textureSize / 2);
		Vec2 targetCenter(seedPoint.targetPosition.x - seedPoint.textureSize / 2,
		                  seedPoint.targetPosition.x - seedPoint.textureSize / 2);
		seedPoint.patch = std::make_unique<MondriaanPatch>(
			sourceCenter,
			targetCenter,
			Vec2(seedPoint.textureSize, seedPoint.textureSize)
		);
	}
}

std::size_t mutations = 3;
std::vector indices = {0, 1, 2, 3, 4, 5};

void SeedPointsTab::mutatePatches() {
	std::vector sourceTextures = {
	screen->editor->pipelineTab->sourceGrayscale->data, screen->editor->pipelineTab->sourceSobel->data
	};
	std::vector targetTextures = {
	screen->editor->pipelineTab->wequalized->data, screen->editor->pipelineTab->targetSobel->data
	};
	std::vector<double> distribution = {screen->settings->intensityWeight, screen->settings->edgeWeight};

	for (const SeedPoint& seedPoint : seedPoints) {
		MondriaanPatch* patch = reinterpret_cast<MondriaanPatch*>(seedPoint.patch.get());
		if (patch == nullptr)
			continue;

		double currentDistance = Match(patch, sourceTextures, targetTextures, distribution).distance;

		int bestMutation = -1;
		double bestDistance = std::numeric_limits<double>::max();
		std::ranges::shuffle(indices, generator);
		for (std::size_t index = 0; index < mutations; index++) {
			MondriaanPatch newPatch = patch->getMutation(indices[index], 5.0);
			double newDistance = Match(patch, sourceTextures, targetTextures, distribution).distance;

			if (newDistance < bestDistance) {
				bestDistance = newDistance;
				bestMutation = indices[index];
			}
		}

		if (bestMutation == -1) {
			Log::debug("skip");
			continue;
		}

		patch->mutate(bestMutation, 1.0);
	}
}

void SeedPointsTab::generateImage() {

	seedPoints.clear();
	std::multimap<double, SeedPoint, std::greater<>> seedPoints;

	for (int i = 0; i < targetSeedPointsGridDivisions; i++) {
		for (int j = 0; j < targetSeedPointsGridDivisions; j++) {
			int xInterval = screen->editor->pipelineTab->saliencyMap->data.cols / targetSeedPointsGridDivisions;
			int yInterval = screen->editor->pipelineTab->saliencyMap->data.rows / targetSeedPointsGridDivisions;
			int xMin = i * screen->editor->pipelineTab->saliencyMap->data.cols / targetSeedPointsGridDivisions;
			int yMin = j * screen->editor->pipelineTab->saliencyMap->data.rows / targetSeedPointsGridDivisions;
			cv::Rect rect(xMin, yMin, xInterval, yInterval);
			cv::Mat subTexture(screen->editor->pipelineTab->saliencyMap->data, rect);

			double value;
			cv::minMaxLoc(subTexture, nullptr, &value, nullptr, nullptr);

			seedPoints.insert(
				std::make_pair(
					value,
					SeedPoint(
						Utils::transform(Vec2(xMin + xInterval / 2, yMin + yInterval / 2),
							target.textureDimension(),
							source.textureDimension()),
						Vec2(xMin + xInterval / 2, yMin + yInterval / 2),
						screen->settings->seedPointSize
					)
				)
			);
		}
	}

	for (const auto& [distance, seedPoint] : seedPoints)
		this->seedPoints.push_back(seedPoint);

	spawnSourceSeedpoints();
	
	cv::Mat output(target.texture->data.rows, target.texture->data.cols, target.texture->data.type());
	for (const auto& sp : this->seedPoints) {
		cv::Rect sourceBounds = sp.sourceBounds(source.texture).cv();
		cv::Rect targetBounds = sp.targetBounds(target.texture).cv();
		sourceBounds.width = Utils::min(sourceBounds.width, targetBounds.width);
		targetBounds.width = Utils::min(sourceBounds.width, targetBounds.width);
		sourceBounds.height = Utils::min(sourceBounds.height, targetBounds.height);
		targetBounds.height = Utils::min(sourceBounds.height, targetBounds.height);

		cv::copyTo(source.texture->data(sourceBounds), output(targetBounds), cv::Mat());
	}

	cv::imshow("Output", output);
}


void SeedPointsTab::onSourceChanged() {
	source.texture = screen->editor->pipelineTab->rotatedSource.get();
	source.aspect = static_cast<double>(source.texture->data.cols) / static_cast<double>(source.texture->data.rows);
	source.features[Canvas::FEATURE_SOBEL] = screen->editor->pipelineTab->sourceSobel.get();
	source.features[Canvas::FEATURE_INT] = screen->editor->pipelineTab->sourceGrayscale.get();
	source.dimension = Canvas::computeDimension(source.textureDimension(), screen->settings->imageSize);
}

void SeedPointsTab::onTargetChanged() {
	target.texture = screen->settings->targetTexture->texture.get();
	target.aspect = static_cast<double>(target.texture->data.cols) / static_cast<double>(target.texture->data.rows);
	target.features[Canvas::FEATURE_SOBEL] = screen->editor->pipelineTab->targetSobel.get();
	target.features[Canvas::FEATURE_INT] = screen->editor->pipelineTab->wequalized.get();
	target.dimension = Canvas::computeDimension(target.textureDimension(), screen->settings->imageSize);
}

void SeedPointsTab::reload() {
	onSourceChanged();
	onTargetChanged();
}

void SeedPointsTab::resetSelection() {
	selectedIndex = -1;
	selectedPoint = Vec2f();
	intersectedIndex = -1;
	intersectedPoint = Vec2f();
}
