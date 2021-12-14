#include "core.h"
#include "seedPoint.h"

#include "graphics/bounds.h"
#include "view/screen.h"

SeedPoint::SeedPoint(const Vec2& sourcePosition, const Vec2& targetPosition, int textureSize) {
	this->sourcePosition = sourcePosition;
	this->targetPosition = targetPosition;
	this->textureSize = textureSize;
}

Bounds SeedPoint::sourceBounds(const Texture* source) const {
	return Bounds::clamped(
		Bounds(sourcePosition, textureSize),
		Bounds(0, 0, source->data.cols, source->data.rows)
	);
}

Bounds SeedPoint::targetBounds(const Texture* target) const {
	return Bounds::clamped(
		Bounds(targetPosition, textureSize),
		Bounds(0, 0, target->data.cols, target->data.rows)
	);
}

void SeedPoint::render(const Canvas& sourceCanvas, const Canvas& targetCanvas, bool intersected, bool selected, bool showConnections, const Color& color) const {
	Vec2 sourceSize = sourceCanvas.toRelativeScreenSpace(Vec2(textureSize, textureSize));
	Vec2 targetSize = targetCanvas.toRelativeScreenSpace(Vec2(textureSize, textureSize));
	float maxTargetSize = Utils::max(targetSize.x, targetSize.y);
	float maxSourceSize = Utils::max(sourceSize.x, sourceSize.y);

	ImGui::GetWindowDrawList()->AddCircleFilled(sourceCanvas.toAbsoluteScreenSpace(sourcePosition).iv(), maxSourceSize / 2, color.u32());
	ImGui::GetWindowDrawList()->AddCircleFilled(targetCanvas.toAbsoluteScreenSpace(targetPosition).iv(), maxTargetSize / 2, color.u32());
	ImGui::GetWindowDrawList()->AddCircle(sourceCanvas.toAbsoluteScreenSpace(sourcePosition).iv(), maxSourceSize / 2, Colors::WHITE.u32(), 12, 1);
	ImGui::GetWindowDrawList()->AddCircle(targetCanvas.toAbsoluteScreenSpace(targetPosition).iv(), maxTargetSize / 2, Colors::WHITE.u32(), 12, 1);

	if (intersected) {
		Bounds(sourceCanvas.toRelativeScreenSpace(sourcePosition), sourceSize.x, sourceSize.y).render(sourceCanvas);
		Bounds(targetCanvas.toRelativeScreenSpace(targetPosition), targetSize.x, targetSize.y).render(targetCanvas);
	}

	if (selected) {
		ImGui::GetWindowDrawList()->AddCircleFilled(sourceCanvas.toAbsoluteScreenSpace(sourcePosition).iv(), (maxSourceSize - 8) / 2, Colors::WHITE.u32());
		ImGui::GetWindowDrawList()->AddCircleFilled(targetCanvas.toAbsoluteScreenSpace(targetPosition).iv(), (maxTargetSize - 8) / 2, Colors::WHITE.u32());
	}

	if (showConnections || selected)
		ImGui::GetWindowDrawList()->AddLine(sourceCanvas.toAbsoluteScreenSpace(sourcePosition).iv(), targetCanvas.toAbsoluteScreenSpace(targetPosition).iv(), Colors::WHITE.u32());
}
