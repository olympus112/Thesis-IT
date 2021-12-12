#include "core.h"
#include "seedPoint.h"

#include "main.h"
#include "graphics/bounds.h"
#include "view/screen.h"

SeedPoint::SeedPoint(const Vec2& sourcePosition, const Vec2& targetPosition) {
	this->sourcePosition = sourcePosition;
	this->targetPosition = targetPosition;
}

void SeedPoint::render(const Canvas& sourceCanvas, const Canvas& targetCanvas, bool intersected, bool selected, bool showConnections, const Color& color) const {
	float radius = 6;

	ImGui::GetWindowDrawList()->AddCircleFilled(sourceCanvas.toAbsoluteScreenSpace(sourcePosition).iv(), screenSize / 2, color.u32());
	ImGui::GetWindowDrawList()->AddCircleFilled(targetCanvas.toAbsoluteScreenSpace(targetPosition).iv(), screenSize / 2, color.u32());
	ImGui::GetWindowDrawList()->AddCircle(sourceCanvas.toAbsoluteScreenSpace(sourcePosition).iv(), screenSize / 2, Colors::WHITE.u32(), 12, 1);
	ImGui::GetWindowDrawList()->AddCircle(targetCanvas.toAbsoluteScreenSpace(targetPosition).iv(), screenSize / 2, Colors::WHITE.u32(), 12, 1);

	if (intersected) {
		Bounds(sourceCanvas.toRelativeScreenSpace(sourcePosition), screenSize).render(sourceCanvas);
		Bounds(targetCanvas.toRelativeScreenSpace(targetPosition), screenSize).render(targetCanvas);
	}

	if (selected) {
		ImGui::GetWindowDrawList()->AddCircleFilled(sourceCanvas.toAbsoluteScreenSpace(sourcePosition).iv(), (screenSize - 8) / 2, Colors::WHITE.u32());
		ImGui::GetWindowDrawList()->AddCircleFilled(targetCanvas.toAbsoluteScreenSpace(targetPosition).iv(), (screenSize - 8) / 2, Colors::WHITE.u32());
	}

	if (showConnections || selected)
		ImGui::GetWindowDrawList()->AddLine(sourceCanvas.toAbsoluteScreenSpace(sourcePosition).iv(), targetCanvas.toAbsoluteScreenSpace(targetPosition).iv(), Colors::WHITE.u32());
}