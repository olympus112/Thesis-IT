#include <core.h>
#include "seedpoints.h"

#include "main.h"
#include "screen.h"
#include "graphics/color.h"
#include "graphics/imgui/widgets.h"

//! -------------
//! SeedPointsTab
//! -------------

SeedPointsTab::SeedPointsTab() = default;

void SeedPointsTab::init() {
}

void SeedPointsTab::update() {
	ImVec2 offset = ImGui::GetMousePos() - (sourceHover ? sourceBox : targetBox).min();

	// Mouse move
	if (sourceHover || targetHover) {
		intersectedIndex = -1;
		intersectedPoint = offset;

		bool tooltip = false;
		for (int index = 0; index < seedPointPairs.size(); index++) {
			const auto& [source, target, patchSize] = seedPointPairs[index];

			if (source.bounds().contains(intersectedPoint) && sourceHover) {
				intersectedIndex = index;
				break;
			}
				
			if (target.bounds().contains(intersectedPoint) && targetHover) {
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

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		seedPointPairs.push_back({
		{offset},
		{offset},
			10,
		});
	}

	// Mouse drag
	if ((sourceDrag || targetDrag) && selectedIndex != -1) {
		Vec2f delta = offset - selectedPoint;
		if (sourceDrag)
			seedPointPairs[selectedIndex].source.position += delta;

		if (targetDrag)
			seedPointPairs[selectedIndex].target.position += delta;

		selectedPoint += delta;
	}

	// Mouse release
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		targetDrag = false;
		sourceDrag = false;
	}
}

void SeedPointsTab::render() {
	ImVec2 canvasSize = ImVec2(350, 350);

	// Source image
	ImGui::image("Source", screen->settings->sourceTexture->asImTexture(), canvasSize);
	sourceBox = {ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMin() + canvasSize)};
	sourceHover = ImGui::IsItemHovered();

	ImGui::SameLine(0, 20);

	// Target image
	ImGui::image("Target", screen->settings->targetTexture->asImTexture(), canvasSize);
	targetBox = {ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMin() + canvasSize)};
	targetHover = ImGui::IsItemHovered();

	// Bounding boxes
	ImGui::GetWindowDrawList()->AddRect(sourceBox.min(), sourceBox.max(), Colors::WHITE.u32(), 0, ImDrawCornerFlags_All,
	                                    2);
	ImGui::GetWindowDrawList()->AddRect(targetBox.min(), targetBox.max(), Colors::WHITE.u32(), 0, ImDrawCornerFlags_All,
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
			ImGui::GetWindowDrawList()->AddLine(ImVec2(sourceBox.minX(), cursor.y),
			                                    ImVec2(sourceBox.minX() + size, cursor.y), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(sourceBox.maxX(), cursor.y),
			                                    ImVec2(sourceBox.maxX() - size, cursor.y), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, sourceBox.minY()),
			                                    ImVec2(cursor.x, sourceBox.minY() + size), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, sourceBox.maxY()),
			                                    ImVec2(cursor.x, sourceBox.maxY() - size), Colors::WHITE.u32(),
			                                    thickness);
		}

		if (targetHover) {
			ImGui::GetWindowDrawList()->AddLine(ImVec2(targetBox.minX(), cursor.y),
			                                    ImVec2(targetBox.minX() + size, cursor.y), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(targetBox.maxX(), cursor.y),
			                                    ImVec2(targetBox.maxX() - size, cursor.y), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, targetBox.minY()),
			                                    ImVec2(cursor.x, targetBox.minY() + size), Colors::WHITE.u32(),
			                                    thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, targetBox.maxY()),
			                                    ImVec2(cursor.x, targetBox.maxY() - size), Colors::WHITE.u32(),
			                                    thickness);
		}
	}

	if (intersectedIndex != -1 || selectedIndex != -1) {
		const auto& seedPointPair = seedPointPairs[intersectedIndex != -1 ? intersectedIndex : selectedIndex];
		auto tooltiptPosition = intersectedIndex != -1 ? ImGui::GetCursorPos() : sourceBox.min() + seedPointPair.source.position;

		Bounds sourcePatch(Vec2f(sourceBox.minX() + seedPointPair.source.position.x, sourceBox.minY() + seedPointPair.source.position.y), screen->settings->patchSize);
		Bounds targetPatch(Vec2f(targetBox.minX() + seedPointPair.target.position.x, targetBox.minY() + seedPointPair.target.position.y), screen->settings->patchSize);
		Bounds sourceUV = sourceBox.subBoundsUV(sourcePatch);
		Bounds targetUV = targetBox.subBoundsUV(targetPatch);

		ImGui::SetCursorPos(tooltiptPosition);

		ImGui::SetNextWindowSize(ImVec2(190, 0));
		ImGui::BeginTooltip();

		ImGui::TextColored(Colors::BLUE.iv4(), "General");
		ImGui::Separator();
		ImGui::Text("Patch size: %.2f", screen->settings->patchSize);
		ImGui::Text("Matching: %.2f", seedPointPair.matching);
		ImGui::Separator();

		ImGui::Columns(2);
		ImGui::SetColumnWidth(-1, 95);
		ImGui::TextColored(Colors::BLUE.iv4(), "Target");
		ImGui::Text("X: %.0f", seedPointPair.source.position.x);
		ImGui::Text("Y: %.0f", seedPointPair.source.position.y);
		ImGui::image("Source", screen->settings->sourceTexture->texture->asImTexture(), ImVec2(80, 80), sourceUV.min(), sourceUV.max());

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, 95);
		ImGui::TextColored(Colors::BLUE.iv4(), "Source");
		ImGui::Text("X: %.0f", seedPointPair.target.position.x);
		ImGui::Text("Y: %.0f", seedPointPair.target.position.y);
		ImGui::image("Target", screen->settings->targetTexture->texture->asImTexture(), ImVec2(80, 80), targetUV.min(), targetUV.max());

		ImGui::Columns(1);

		ImGui::EndTooltip();
	}

	// Render seedPoints
	for (int index = 0; index < seedPointPairs.size(); index++) {
		seedPointPairs[index].render(sourceBox, targetBox, intersectedIndex == index, selectedIndex == index);
	}
}

//! -------------
//! SeedPointPair
//! -------------

void SeedPointPair::render(const Bounds& sourceBox, const Bounds& targetBox, bool intersected, bool selected) const {
	source.render(sourceBox.min(), intersected, selected);
	target.render(targetBox.min(), intersected, selected);

	Bounds(source.position, screen->settings->patchSize).render(sourceBox.min(), Colors::RED);
	Bounds(target.position, screen->settings->patchSize).render(targetBox.min(), Colors::RED);

	if (intersected || selected)
		ImGui::GetWindowDrawList()->AddLine(sourceBox.min() + source.position, targetBox.min() + target.position,
		                                    ImGui::ColorConvertFloat4ToU32(ImVec4(1.0, 1.0, 1.0, 0.5)));
}

//! ---------
//! SeedPoint
//! ---------


SeedPoint::SeedPoint(const Vec2f& position) {
	this->position = position;
}

Bounds SeedPoint::bounds() const {
	return Bounds(position, 20);
}

void SeedPoint::render(const Vec2f& offset, bool intersected, bool selected) const {
	float radius = 6;
	ImGui::GetWindowDrawList()->AddCircleFilled((offset + position).asImVec(), radius, Colors::RED.u32());
	ImGui::GetWindowDrawList()->AddCircle((offset + position).asImVec(), radius, Colors::WHITE.u32(), 12, 2);

	if (intersected)
		bounds().render(offset);

	if (selected)
		ImGui::GetWindowDrawList()->AddCircleFilled((offset + position).asImVec(), radius - 4, Colors::WHITE.u32());
}

//! ------
//! Bounds
//! ------

Bounds::Bounds() = default;

Bounds::Bounds(const ImRect& rect) {
	this->rect = rect;
}

Bounds::Bounds(const Vec2f& position, float size) {
	float halfSize = size / 2.0f;
	this->rect = ImRect(position.x - halfSize, position.y - halfSize, position.x + halfSize, position.y + halfSize);
}

void Bounds::render(const Vec2f& offset, const Color& color) const {
	ImGui::GetWindowDrawList()->AddRect(offset + min(), offset + max(), color.u32(), 0,
	                                    ImDrawCornerFlags_All);
}
