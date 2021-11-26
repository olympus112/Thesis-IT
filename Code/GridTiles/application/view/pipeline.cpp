#include <core.h>
#include "pipeline.h"

#include "main.h"
#include "screen.h"
#include "graphics/renderer.h"
#include "util/imageUtils.h"

PipelineTab::PipelineTab() = default;

void PipelineTab::init() {
	rgbHistogram = std::make_unique<Texture>();
	grayscale = std::make_unique<Texture>();
	grayscaleHistogram = std::make_unique<Texture>();
	equalized = std::make_unique<Texture>();
	equalizedHistogram = std::make_unique<Texture>();
}

void PipelineTab::update() {
}

void PipelineTab::render() {
	Render::Image("Source", screen->settings->sourceTexture->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	float cursorX = ImGui::GetCursorPosX();
	Render::Image("Intensity", grayscale->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Equalized", equalized->asImTexture(), Globals::imageSize);
	ImGui::NewLine();
	ImGui::SetCursorPosX(cursorX);
	Render::Image("Target", screen->settings->targetTexture->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow({ 0, Globals::imageWidth / 2 }, { Globals::arrowWidth, 0 });

	// Histograms
	Render::Image("RGB hist", rgbHistogram->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Grayscale hist", grayscaleHistogram->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Equalized hist", equalizedHistogram->asImTexture(), Globals::imageSize);
}

void PipelineTab::reload() {
	// RGB histogram
	ImageUtils::computeHistogram(screen->settings->sourceTexture->texture.get(), rgbHistogram.get());

	// Grayscale
	ImageUtils::computeGrayscale(screen->settings->sourceTexture->texture.get(), grayscale.get());

	// Calculate grayscale histogram
	ImageUtils::computeHistogram(grayscale.get(), grayscaleHistogram.get(), false);

	// Calculate equalization
	ImageUtils::computeEqualization(grayscale.get(), nullptr, equalized.get());

	// Calculate equalization histogram
	ImageUtils::computeHistogram(equalized.get(), equalizedHistogram.get(), false);

}
