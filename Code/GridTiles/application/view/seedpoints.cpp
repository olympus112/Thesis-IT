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

void SeedPointsTab::init() {

}

void SeedPointsTab::update() {
	ImVec2 offset = ImGui::GetMousePos() - (source.hover ? source : target).offset;

	// Mouse move
	if (source.hover || target.hover) {
		intersectedIndex = -1;
		intersectedPoint = offset;

		for (int index = 0; index < seedPoints.size(); index++) {
			const auto& [sourcePosition, targetPosition] = seedPoints[index];

			if (source.covers(source.toScreenSpace(sourcePosition), intersectedPoint) && source.hover) {
				intersectedIndex = index;
				break;
			}

			if (target.covers(target.toScreenSpace(targetPosition), intersectedPoint) && target.hover) {
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

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && (source.hover || target.drag)) {
		SeedPoint seedpoint(
			source.toTextureSpace(offset),
			target.toTextureSpace(offset)
		);

		seedPoints.push_back(seedpoint);
	}

	// Mouse drag
	if ((source.drag || target.drag) && selectedIndex != -1) {
		Vec2 delta = offset - selectedPoint;
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

void SeedPointsTab::render() {
	ImGui::Begin("SeedPoints");

	// Source image
	ImGui::image("Source", source.texture->it(), source.dimension.iv());
	source.offset = ImGui::GetItemRectMin();
	source.hover = ImGui::IsItemHovered();

	ImGui::SameLine(0, 20);

	// Target image
	ImGui::image("Target", target.texture->it());
	target.offset = ImGui::GetItemRectMin();
	target.hover = ImGui::IsItemHovered();

	// Bounding boxes
	ImGui::GetWindowDrawList()->AddRect(source.min().iv(), source.max().iv(), Colors::WHITE.u32(), 0,
	                                    ImDrawCornerFlags_All,
	                                    2);
	ImGui::GetWindowDrawList()->AddRect(target.min().iv(), target.max().iv(), Colors::WHITE.u32(), 0,
	                                    ImDrawCornerFlags_All,
	                                    2);

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
	}

	// Render seedPoints
	/*float maxMatch = 0;
	for (auto& seedPointPair : seedPoints)
		maxMatch = seedPointPair.matching > maxMatch ? seedPointPair.matching : maxMatch;*/

	// 

	for (int index = 0; index < seedPoints.size(); index++)
		seedPoints[index].render(source, target, intersectedIndex == index, selectedIndex == index);

	ImGui::End();

	// Seedpoint viewer
	ImGui::Begin("Seedpoint viewer");
	/*for (int index = 0; index < seedPoints.size(); index++) {
		auto& seedPoint = seedPoints[index];
		auto sourceScreenPosition = sourceBox.min() + Utils::textureToScreenSpace(seedPoint.sourcePosition,
			screen->settings->sourceTexture->texture->data, screen->settings->imageSize);
		auto targetScreenPosition = targetBox.min() + Utils::textureToScreenSpace(seedPoint.targetPosition,
			screen->settings->targetTexture->texture->data, screen->settings->imageSize);
		Bounds sourcePatch(sourceScreenPosition, seedPoint.size);
		Bounds targetPatch(targetScreenPosition, seedPoint.size);
		Bounds sourceUV = sourceBox.subBoundsUV(sourcePatch);
		Bounds targetUV = targetBox.subBoundsUV(targetPatch);

		float size = 80.0f;
		ImGui::TextColored(Colors::BLUE.iv4(), "Patch #%d", index + 1);
		if (ImGui::SliderInt(std::format("Patch size##{}", index).c_str(), &seedPoint.size, 1, 80)) {
			seedPoints[index].calculateMatch(
				{screen->editor->pipelineTab->sourceGrayscale->data, screen->editor->pipelineTab->sourceSobel->data},
				{screen->editor->pipelineTab->wequalized->data, screen->editor->pipelineTab->targetSobel->data},
				{screen->settings->intensityWeight, screen->settings->edgeWeight});
		}

		ImGui::Columns(4, 0, false);
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Source");
		ImGui::Image(screen->editor->pipelineTab->rotatedSource->asImTexture(), ImVec2(size, size),
		             sourceUV.min().iv(),
		             sourceUV.max().iv());
		ImGui::NextColumn();
		ImGui::Text("");
		ImGui::Text("X: %.0f", seedPoint.source.position.x);
		ImGui::Text("Y: %.0f", seedPoint.source.position.y);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(-1, size + 10);
		ImGui::TextColored(Colors::BLUE.iv4(), "Target");
		ImGui::Image(screen->settings->targetTexture->texture->asImTexture(), ImVec2(size, size),
		             targetUV.min().iv(),
		             targetUV.max().iv());
		ImGui::NextColumn();
		ImGui::Text("");
		ImGui::Text("X: %.0f", seedPoint.target.position.x);
		ImGui::Text("Y: %.0f", seedPoint.target.position.y);

		ImGui::NextColumn();
		ImGui::Image(screen->editor->pipelineTab->sourceGrayscale->asImTexture(), ImVec2(size, size),
		             sourceUV.min().iv(),
		             sourceUV.max().iv());
		ImGui::NextColumn();
		ImGui::Image(screen->editor->pipelineTab->sourceSobel->asImTexture(), ImVec2(size, size),
		             sourceUV.min().iv(),
		             sourceUV.max().iv());
		ImGui::NextColumn();
		ImGui::Image(screen->editor->pipelineTab->wequalized->asImTexture(), ImVec2(size, size),
		             targetUV.min().iv(),
		             targetUV.max().iv());
		ImGui::NextColumn();
		ImGui::Image(screen->editor->pipelineTab->targetSobel->asImTexture(), ImVec2(size, size),
		             targetUV.min().iv(),
		             targetUV.max().iv());
		ImGui::NextColumn();

		ImGui::Columns(1);

		ImGui::Text("Matching: %.2f", seedPoint.matching);

		ImGui::Separator();
	}*/
	ImGui::End();
}

void SeedPointsTab::onSourceChanged() {
	source.texture = screen->editor->pipelineTab->rotatedSource.get();
	source.aspect = static_cast<double>(source.texture->data.cols) / static_cast<double>(source.texture->data.rows);
	source.dimension = Canvas::computeDimension(source.textureDimension(), screen->settings->imageSize);
}

void SeedPointsTab::onTargetChanged() {
	target.texture = screen->settings->targetTexture->texture.get();
	target.aspect = static_cast<double>(target.texture->data.cols) / static_cast<double>(target.texture->data.rows);
	target.dimension = Canvas::computeDimension(target.textureDimension(), screen->settings->imageSize);
}

void SeedPointsTab::reload() {
	onSourceChanged();
	onTargetChanged();
}
