#include <core.h>
#include "pipeline.h"

#include "main.h"
#include "screen.h"
#include "graphics/renderer.h"
#include "util/imageUtils.h"

PipelineTab::PipelineTab() = default;

void PipelineTab::init() {
	sourceHistogram = std::make_unique<Texture>();
	sourceCDF = std::make_unique<Texture>();
	targetHistogram = std::make_unique<Texture>();
	targetCDF = std::make_unique<Texture>();
	sourceGrayscale = std::make_unique<Texture>();
	sourceGrayscaleHistogram = std::make_unique<Texture>();
	sourceGrayscaleCDF = std::make_unique<Texture>();
	targetGrayscale = std::make_unique<Texture>();
	targetGrayscaleHistogram = std::make_unique<Texture>();
	targetGrayscaleCDF = std::make_unique<Texture>();
	equalized = std::make_unique<Texture>();
	equalizedHistogram = std::make_unique<Texture>();
	equalizedCDF = std::make_unique<Texture>();
}

void PipelineTab::update() {
}

void PipelineTab::render() {
	Render::Image("Source", screen->settings->sourceTexture->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Source int", sourceGrayscale->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Equalized", equalized->asImTexture(), Globals::imageSize);
	ImGui::NewLine();
	Render::Image("Target", screen->settings->targetTexture->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Target int", targetGrayscale->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow({ 0, Globals::imageWidth / 2 }, { Globals::arrowWidth, 0 });

	// Histograms
	Render::Image("Source hist", sourceHistogram->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Source int hist", sourceGrayscaleHistogram->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Equalized hist", equalizedHistogram->asImTexture(), Globals::imageSize);
	ImGui::NewLine();
	Render::Image("Target hist", targetHistogram->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Target int hist", targetGrayscaleHistogram->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow({ 0, Globals::imageWidth / 2 }, { Globals::arrowWidth, 0 });

	// CDF
	Render::Image("Source cdf", sourceCDF->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Grayscale cdf", sourceGrayscaleCDF->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Equalized cdf", equalizedCDF->asImTexture(), Globals::imageSize);
	ImGui::NewLine();
	Render::Image("Target cdf", targetCDF->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Target int cdf", targetGrayscaleCDF->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow({ 0, Globals::imageWidth / 2 }, { Globals::arrowWidth, 0 });
}

void PipelineTab::reload() {
	// Source histogram
	ImageUtils::renderHistogram(screen->settings->sourceTexture->texture.get(), sourceHistogram.get());
	ImageUtils::renderCDF(screen->settings->sourceTexture->texture.get(), sourceCDF.get());

	// Target histogram
	ImageUtils::renderHistogram(screen->settings->targetTexture->texture.get(), targetHistogram.get());
	ImageUtils::renderCDF(screen->settings->targetTexture->texture.get(), targetCDF.get());

	// Grayscale
	ImageUtils::computeGrayscale(screen->settings->sourceTexture->texture.get(), sourceGrayscale.get());
	ImageUtils::computeGrayscale(screen->settings->targetTexture->texture.get(), targetGrayscale.get());

	// Calculate grayscale histogram
	ImageUtils::renderHistogram(sourceGrayscale.get(), sourceGrayscaleHistogram.get(), false);
	ImageUtils::renderHistogram(targetGrayscale.get(), targetGrayscaleHistogram.get(), false);
	ImageUtils::renderCDF(sourceGrayscale.get(), sourceGrayscaleCDF.get(), false);
	ImageUtils::renderCDF(targetGrayscale.get(), targetGrayscaleCDF.get(), false);

	// Calculate equalization
	ImageUtils::renderEqualization(sourceGrayscale.get(), targetGrayscale.get(), equalized.get());

	// Calculate equalization histogram
	ImageUtils::renderHistogram(equalized.get(), equalizedHistogram.get(), false);
	ImageUtils::renderCDF(equalized.get(), equalizedCDF.get(), false);

}
