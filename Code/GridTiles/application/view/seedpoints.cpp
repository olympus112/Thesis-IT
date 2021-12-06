#include <core.h>
#include "seedpoints.h"
#include <format>

#include "main.h"
#include "screen.h"
#include "graphics/color.h"
#include "graphics/imgui/widgets.h"
#include "math/utils.h"

//! -------------
//! SeedPointsTab
//! -------------

SeedPointsTab::SeedPointsTab() = default;

void SeedPointsTab::init() {
}

Vec2f screenToTexture(const Vec2f& vector, const cv::Mat& texture, float width = screen->settings->imageSize) {
	float rows = static_cast<float>(texture.size[0]);
	float cols = static_cast<float>(texture.size[1]);
	float height = width / cols * rows;
	return Vec2f(
		vector.x / width * cols,
		vector.y / height * rows
	);
}

Vec2f textureToScreen(const Vec2f& vector, const cv::Mat& texture, float width = screen->settings->imageSize) {
	float rows = static_cast<float>(texture.size[0]);
	float cols = static_cast<float>(texture.size[1]);
	float height = width / cols * rows;
	return Vec2f(
		vector.x / cols * width,
		vector.y / rows * height
	);
}

void SeedPointsTab::update() {

	ImVec2 offset = ImGui::GetMousePos() - (sourceHover ? sourceBox : targetBox).min();

	// Mouse move
	if (sourceHover || targetHover) {
		intersectedIndex = -1;
		intersectedPoint = offset;

		for (int index = 0; index < seedPointPairs.size(); index++) {
			const auto& [source, target, patchSize, matching] = seedPointPairs[index];

			if (Bounds(textureToScreen(source.position, screen->settings->sourceTexture->texture->data), 20).contains(intersectedPoint) &&
				sourceHover) {
				intersectedIndex = index;
				break;
			}

			if (Bounds(textureToScreen(target.position, screen->settings->targetTexture->texture->data), 20).contains(intersectedPoint) &&
				targetHover) {
				intersectedIndex = index;
				break;
			}
		}
	}
	else {
		sourceDrag = false;
		targetDrag = false;
	}

	// Mouse press
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		if (sourceHover || targetHover) {
			selectedPoint = intersectedPoint;
			selectedIndex = intersectedIndex;

			if (targetHover)
				targetDrag = true;

			if (sourceHover)
				sourceDrag = true;
		}
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && (sourceHover || targetDrag)) {
		SeedPointPair pair = {
		{screenToTexture(offset, screen->settings->sourceTexture->texture->data)},
		{screenToTexture(offset, screen->settings->targetTexture->texture->data)},
		20,
			0.0
		};

		pair.calculateMatch(
			{screen->editor->pipelineTab->sourceGrayscale->data, screen->editor->pipelineTab->sourceSobel->data},
			{screen->editor->pipelineTab->wequalized->data, screen->editor->pipelineTab->targetSobel->data},
			{screen->settings->intensityWeight, screen->settings->edgeWeight});
		seedPointPairs.push_back(pair);
	}

	// Mouse drag
	if ((sourceDrag || targetDrag) && selectedIndex != -1) {
		Vec2f delta = offset - selectedPoint;
		if (sourceDrag)
			seedPointPairs[selectedIndex].source.position += screenToTexture(
				delta, screen->settings->sourceTexture->texture->data);

		if (targetDrag)
			seedPointPairs[selectedIndex].target.position += screenToTexture(
				delta, screen->settings->targetTexture->texture->data);

		selectedPoint += delta;

		seedPointPairs[selectedIndex].calculateMatch(
			{screen->editor->pipelineTab->sourceGrayscale->data, screen->editor->pipelineTab->sourceSobel->data},
			{screen->editor->pipelineTab->wequalized->data, screen->editor->pipelineTab->targetSobel->data},
			{screen->settings->intensityWeight, screen->settings->edgeWeight});
	}

	// Mouse release
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		targetDrag = false;
		sourceDrag = false;
	}
}

void SeedPointsTab::render() {
	ImGui::Begin("SeedPoints");

	ImVec2 canvasSize = ImVec2(screen->settings->imageSize, screen->settings->imageSize);

	// Source image
	ImGui::image("Source", screen->editor->pipelineTab->rotatedSource->asImTexture(), canvasSize);
	sourceBox = {ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMin() + canvasSize)};
	sourceHover = ImGui::IsItemHovered();

	ImGui::SameLine(0, 20);

	// Target image
	ImGui::image("Target", screen->settings->targetTexture->asImTexture(), canvasSize);
	targetBox = {ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMin() + canvasSize)};
	targetHover = ImGui::IsItemHovered();

	// Bounding boxes
	ImGui::GetWindowDrawList()->AddRect(sourceBox.min().asImVec(), sourceBox.max().asImVec(), Colors::WHITE.u32(), 0, ImDrawCornerFlags_All,
	                                    2);
	ImGui::GetWindowDrawList()->AddRect(targetBox.min().asImVec(), targetBox.max().asImVec(), Colors::WHITE.u32(), 0, ImDrawCornerFlags_All,
	                                    2);

	// Cursor
	if (sourceHover || targetHover) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		float size = 7;
		float thickness = 3;
		ImVec2 cursor = ImGui::GetMousePos();
		ImGui::GetWindowDrawList()->AddLine(cursor - ImVec2(size, 0), cursor + ImVec2(size, 0), Colors::WHITE.u32(),
		                                    thickness);
		ImGui::GetWindowDrawList()->AddLine(cursor - ImVec2(0, size), cursor + ImVec2(0, size), Colors::WHITE.u32(),
		                                    thickness);

		if (sourceHover) {
			ImGui::GetWindowDrawList()->AddLine(ImVec2(sourceBox.minX, cursor.y),
			                                    ImVec2(sourceBox.minX + size, cursor.y), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(sourceBox.maxX, cursor.y),
			                                    ImVec2(sourceBox.maxX - size, cursor.y), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, sourceBox.minY),
			                                    ImVec2(cursor.x, sourceBox.minY + size), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, sourceBox.maxY),
			                                    ImVec2(cursor.x, sourceBox.maxY - size), Colors::WHITE.u32(),
			                                    thickness);
		}

		if (targetHover) {
			ImGui::GetWindowDrawList()->AddLine(ImVec2(targetBox.minX, cursor.y),
			                                    ImVec2(targetBox.minX + size, cursor.y), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(targetBox.maxX, cursor.y),
			                                    ImVec2(targetBox.maxX - size, cursor.y), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, targetBox.minY),
			                                    ImVec2(cursor.x, targetBox.minY + size), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, targetBox.maxY),
			                                    ImVec2(cursor.x, targetBox.maxY - size), Colors::WHITE.u32(),
			                                    thickness);
		}
	}

	// Tooltip
	if (intersectedIndex != -1 || selectedIndex != -1) {
		const auto& seedPointPair = seedPointPairs[intersectedIndex != -1 ? intersectedIndex : selectedIndex];
		auto sourceScreenPosition = sourceBox.min() + textureToScreen(seedPointPair.source.position,
		                                                              screen->settings->sourceTexture->texture->data);
		auto targetScreenPosition = targetBox.min() + textureToScreen(seedPointPair.target.position,
		                                                              screen->settings->targetTexture->texture->data);
		auto tooltiptPosition = intersectedIndex != -1
			                        ? ImGui::GetCursorPos()
			                        : sourceBox.min() + seedPointPair.source.position;

		Bounds sourcePatch(sourceScreenPosition, seedPointPair.size);
		Bounds targetPatch(targetScreenPosition, seedPointPair.size);
		Bounds sourceUV = sourceBox.subBoundsUV(sourcePatch);
		Bounds targetUV = targetBox.subBoundsUV(targetPatch);

		ImGui::SetCursorPos(tooltiptPosition.asImVec());

		ImGui::SetNextWindowSize(ImVec2(190, 0));
		ImGui::BeginTooltip();

		ImGui::TextColored(Colors::BLUE.iv4(), "General");
		ImGui::Separator();
		ImGui::Text("Patch size: %.2f", seedPointPair.size);
		ImGui::Text("Matching: %.2f", seedPointPair.matching);
		ImGui::Separator();

		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, 95);
		ImGui::TextColored(Colors::BLUE.iv4(), "Source");
		ImGui::Text("X: %.0f", seedPointPair.source.position.x);
		ImGui::Text("Y: %.0f", seedPointPair.source.position.y);
		ImGui::image("Source", screen->editor->pipelineTab->rotatedSource->asImTexture(), ImVec2(80, 80),
		             sourceUV.min().asImVec(), sourceUV.max().asImVec());

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, 95);
		ImGui::TextColored(Colors::BLUE.iv4(), "Target");
		ImGui::Text("X: %.0f", seedPointPair.target.position.x);
		ImGui::Text("Y: %.0f", seedPointPair.target.position.y);
		ImGui::image("Target", screen->settings->targetTexture->texture->asImTexture(), ImVec2(80, 80), targetUV.min().asImVec(),
		             targetUV.max().asImVec());

		ImGui::Columns(1);

		ImGui::EndTooltip();
	}

	// Render seedPoints
	float maxMatch = 0;
	for (auto& seedPointPair : seedPointPairs) 
		maxMatch = seedPointPair.matching > maxMatch ? seedPointPair.matching : maxMatch;

	for (int index = 0; index < seedPointPairs.size(); index++) 
		seedPointPairs[index].render(sourceBox, targetBox, intersectedIndex == index, selectedIndex == index, Color::blend(Colors::GREEN, Colors::RED, seedPointPairs[index].matching / maxMatch));
	
	

	ImGui::End();

	// Seedpoint viewer
	ImGui::Begin("Seedpoint viewer");
	for (int index = 0; index < seedPointPairs.size(); index++) {
		auto& seedPointPair = seedPointPairs[index];
		auto sourceScreenPosition = sourceBox.min() + textureToScreen(seedPointPair.source.position,
		                                                              screen->settings->sourceTexture->texture->data);
		auto targetScreenPosition = targetBox.min() + textureToScreen(seedPointPair.target.position,
		                                                              screen->settings->targetTexture->texture->data);
		Bounds sourcePatch(sourceScreenPosition, seedPointPair.size);
		Bounds targetPatch(targetScreenPosition, seedPointPair.size);
		Bounds sourceUV = sourceBox.subBoundsUV(sourcePatch);
		Bounds targetUV = targetBox.subBoundsUV(targetPatch);

		float size = 80.0f;
		ImGui::TextColored(Colors::BLUE.iv4(), "Patch #%d", index + 1);
		if (ImGui::SliderInt(std::format("Patch size##{}", index).c_str(), &seedPointPair.size, 1, 80)) {
			seedPointPairs[index].calculateMatch(
				{screen->editor->pipelineTab->sourceGrayscale->data, screen->editor->pipelineTab->sourceSobel->data},
				{screen->editor->pipelineTab->wequalized->data, screen->editor->pipelineTab->targetSobel->data},
				{screen->settings->intensityWeight, screen->settings->edgeWeight});
		}

		ImGui::Columns(4, 0, false);
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Source");
		ImGui::Image(screen->editor->pipelineTab->rotatedSource->asImTexture(), ImVec2(size, size), sourceUV.min().asImVec(),
		             sourceUV.max().asImVec());
		ImGui::NextColumn();
		ImGui::Text("");
		ImGui::Text("X: %.0f", seedPointPair.source.position.x);
		ImGui::Text("Y: %.0f", seedPointPair.source.position.y);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Target");
		ImGui::Image(screen->settings->targetTexture->texture->asImTexture(), ImVec2(size, size), targetUV.min().asImVec(),
		             targetUV.max().asImVec());
		ImGui::NextColumn();
		ImGui::Text("");
		ImGui::Text("X: %.0f", seedPointPair.target.position.x);
		ImGui::Text("Y: %.0f", seedPointPair.target.position.y);

		ImGui::NextColumn();
		ImGui::Image(screen->editor->pipelineTab->sourceGrayscale->asImTexture(), ImVec2(size, size), sourceUV.min().asImVec(),
		             sourceUV.max().asImVec());
		ImGui::NextColumn();
		ImGui::Image(screen->editor->pipelineTab->sourceSobel->asImTexture(), ImVec2(size, size), sourceUV.min().asImVec(),
		             sourceUV.max().asImVec());
		ImGui::NextColumn();
		ImGui::Image(screen->editor->pipelineTab->wequalized->asImTexture(), ImVec2(size, size), targetUV.min().asImVec(),
		             targetUV.max().asImVec());
		ImGui::NextColumn();
		ImGui::Image(screen->editor->pipelineTab->targetSobel->asImTexture(), ImVec2(size, size), targetUV.min().asImVec(),
		             targetUV.max().asImVec());
		ImGui::NextColumn();

		ImGui::Columns(1);

		ImGui::Text("Matching: %.2f", seedPointPair.matching);

		ImGui::Separator();
	}
	ImGui::End();
}

void SeedPointsTab::onRecalculateMatching() {
	for (auto& pair : seedPointPairs) {
		pair.calculateMatch(
			{ screen->editor->pipelineTab->sourceGrayscale->data, screen->editor->pipelineTab->sourceSobel->data },
			{ screen->editor->pipelineTab->wequalized->data, screen->editor->pipelineTab->targetSobel->data },
			{ screen->settings->intensityWeight, screen->settings->edgeWeight });
	}
}

//! -------------
//! SeedPointPair
//! -------------

void SeedPointPair::render(const Bounds& sourceBox, const Bounds& targetBox, bool intersected, bool selected, const Color& color) const {
	source.render(sourceBox.min().asImVec(), [](const Vec2f& position) {
		return textureToScreen(position, screen->settings->sourceTexture->texture->data).asImVec();
	}, intersected, selected);
	target.render(targetBox.min().asImVec(), [](const Vec2f& position) {
		return textureToScreen(position, screen->settings->targetTexture->texture->data).asImVec();
	}, intersected, selected);

	auto sourceScreenPosition = textureToScreen(source.position, screen->settings->sourceTexture->texture->data);
	auto targetScreenPosition = textureToScreen(target.position, screen->settings->targetTexture->texture->data);
	Bounds(sourceScreenPosition, size).render(sourceBox.min(), color);
	Bounds(targetScreenPosition, size).render(targetBox.min(), color);

	if (intersected || selected)
		ImGui::GetWindowDrawList()->AddLine(sourceBox.min().asImVec() + sourceScreenPosition,
		                                    targetBox.min().asImVec() + targetScreenPosition,
		                                    ImGui::ColorConvertFloat4ToU32(ImVec4(1.0, 1.0, 1.0, 0.5)));
}

void SeedPointPair::calculateMatch(const std::vector<cv::Mat>& sourceTextures,
                                   const std::vector<cv::Mat>& targetTextures, const std::vector<float>& distribution) {
	matching = 0.0;

	for (std::size_t index = 0; index < sourceTextures.size(); index++) {
		cv::Mat sourcePatch = source.calculatePatch(sourceTextures[index], size);
		cv::Mat targetPatch = target.calculatePatch(targetTextures[index], size);

		matching += distribution[index] * cv::norm(sourcePatch, targetPatch) / static_cast<float>(sourceTextures[index].size[0] * sourceTextures[index].size[1]) * 1000.0f;
	}
}

//! ---------
//! SeedPoint
//! ---------


SeedPoint::SeedPoint(const Vec2f& position) {
	this->position = position;
}

cv::Mat SeedPoint::calculatePatch(const cv::Mat& texture, int size) {
	int textureRows = texture.size[0];
	int textureCols = texture.size[1];

	int minX = Utils::clamp<int>(floor(position.x) - size, 0, textureCols);
	int maxX = Utils::clamp<int>(floor(position.x) + size, 0, textureCols);
	int minY = Utils::clamp<int>(floor(position.y) - size, 0, textureRows);
	int maxY = Utils::clamp<int>(floor(position.y) + size, 0, textureRows);

	cv::Range rows(minY, maxY);
	cv::Range cols(minX, maxX);

	return texture(rows, cols);
}

void SeedPoint::render(const ImVec2& offset, ImVec2 (*transform)(const Vec2f&), bool intersected, bool selected) const {
	float radius = 6;

	ImGui::GetWindowDrawList()->AddCircleFilled(offset + transform(position), radius, Colors::RED.u32());
	ImGui::GetWindowDrawList()->AddCircle(offset + transform(position), radius, Colors::WHITE.u32(), 12, 2);

	if (intersected)
		Bounds(transform(position), 20).render(offset);

	if (selected)
		ImGui::GetWindowDrawList()->AddCircleFilled(offset + transform(position), radius - 4, Colors::WHITE.u32());
}