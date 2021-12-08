#include "core.h"
#include "seedPoint.h"

#include "main.h"
#include "graphics/bounds.h"
#include "view/screen.h"


//! -------------
//! SeedPointPair
//! -------------

//SeedPointPair::SeedPointPair(const SeedPoint& source, const SeedPoint& target, int size, float matching) {
//	this->source = source;
//	this->target = target;
//	this->size = size;
//	this->matching = matching;
//}
//
//void SeedPointPair::render(const Bounds& sourceBox, const Bounds& targetBox, bool intersected, bool selected, const Color& color) const {
//	source.render(sourceBox.min().iv(), [] (const Vec2f& position) {
//		return Utils::textureToScreenSpace(position, screen->settings->sourceTexture->texture->data, screen->settings->imageSize).iv();
//		}, intersected, selected);
//	target.render(targetBox.min().iv(), [] (const Vec2f& position) {
//		return Utils::textureToScreenSpace(position, screen->settings->targetTexture->texture->data, screen->settings->imageSize).iv();
//		}, intersected, selected);
//
//	auto sourceScreenPosition = Utils::textureToScreenSpace(source.position, screen->settings->sourceTexture->texture->data, screen->settings->imageSize);
//	auto targetScreenPosition = Utils::textureToScreenSpace(target.position, screen->settings->targetTexture->texture->data, screen->settings->imageSize);
//	Bounds(sourceScreenPosition, size).render(sourceBox.min(), color);
//	Bounds(targetScreenPosition, size).render(targetBox.min(), color);
//
//	if (intersected || selected)
//		ImGui::GetWindowDrawList()->AddLine(sourceBox.min().iv() + sourceScreenPosition,
//			targetBox.min().iv() + targetScreenPosition,
//			ImGui::ColorConvertFloat4ToU32(ImVec4(1.0, 1.0, 1.0, 0.5)));
//}
//
//void SeedPointPair::calculateMatch(const std::vector<cv::Mat>& sourceTextures,
//	const std::vector<cv::Mat>& targetTextures, const std::vector<float>& distribution) {
//	matching = 0.0;
//
//	for (std::size_t index = 0; index < sourceTextures.size(); index++) {
//		cv::Mat sourcePatch = source.calculatePatch(sourceTextures[index], size);
//		cv::Mat targetPatch = target.calculatePatch(targetTextures[index], size);
//
//		matching += distribution[index] * cv::norm(sourcePatch, targetPatch) / static_cast<float>(sourceTextures[index].size[0] * sourceTextures[index].size[1]) * 1000.0f;
//	}
//}

//! ---------
//! SeedPoint
//! ---------


SeedPoint::SeedPoint(const Vec2& sourcePosition, const Vec2& targetPosition) {
	this->sourcePosition = sourcePosition;
	this->targetPosition = targetPosition;
}

//cv::Mat SeedPoint::calculatePatch(const cv::Mat& texture, int size) {
//	int textureRows = texture.size[0];
//	int textureCols = texture.size[1];
//
//	int minX = Utils::clamp<int>(floor(position.x) - size, 0, textureCols);
//	int maxX = Utils::clamp<int>(floor(position.x) + size, 0, textureCols);
//	int minY = Utils::clamp<int>(floor(position.y) - size, 0, textureRows);
//	int maxY = Utils::clamp<int>(floor(position.y) + size, 0, textureRows);
//
//	cv::Range rows(minY, maxY);
//	cv::Range cols(minX, maxX);
//
//	return texture(rows, cols);
//}

void SeedPoint::render(const Canvas& sourceCanvas, const Canvas& targetCanvas, bool intersected, bool selected) const {
	float radius = 6;

	ImGui::GetWindowDrawList()->AddCircleFilled(sourceCanvas.toAbsoluteScreenSpace(sourcePosition).iv(), radius, Colors::RED.u32());
	ImGui::GetWindowDrawList()->AddCircleFilled(targetCanvas.toAbsoluteScreenSpace(targetPosition).iv(), radius, Colors::RED.u32());
	ImGui::GetWindowDrawList()->AddCircle(sourceCanvas.toAbsoluteScreenSpace(sourcePosition).iv(), radius, Colors::WHITE.u32(), 12, 2);
	ImGui::GetWindowDrawList()->AddCircle(targetCanvas.toAbsoluteScreenSpace(targetPosition).iv(), radius, Colors::WHITE.u32(), 12, 2);

	if (intersected) {
		Bounds(sourceCanvas.toScreenSpace(sourcePosition), 20).render(sourceCanvas);
		Bounds(targetCanvas.toScreenSpace(targetPosition), 20).render(targetCanvas);
	}

	if (selected) {
		ImGui::GetWindowDrawList()->AddCircleFilled(sourceCanvas.toAbsoluteScreenSpace(sourcePosition).iv(), radius - 4, Colors::WHITE.u32());
		ImGui::GetWindowDrawList()->AddCircleFilled(targetCanvas.toAbsoluteScreenSpace(targetPosition).iv(), radius - 4, Colors::WHITE.u32());
	}
}