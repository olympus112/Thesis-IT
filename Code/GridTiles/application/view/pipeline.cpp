#include <core.h>
#include "pipeline.h"

#include "main.h"
#include "screen.h"
#include "graphics/imgui/widgets.h"
#include "util/imageUtils.h"

static int pipeline = 0;
static std::vector pipelines = {"Default", "Histogram", "CDF", "Blur"};

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

	blur = std::make_unique<Texture>();
	sobelX = std::make_unique<Texture>();
	sobelY = std::make_unique<Texture>();
	sobelXY = std::make_unique<Texture>();
	canny = std::make_unique<Texture>();
}

void PipelineTab::update() {
}

void PipelineTab::render() {
	for (int index = 0; index < pipelines.size(); index++)
		if (ImGui::RadioButton(pipelines[index], index == pipeline))
			pipeline = index;

	ImGui::Separator();

	// Default
	if (pipeline == 0) {
		ImGui::image("Target", screen->settings->targetTexture->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target int", targetGrayscale->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Equalized", equalized->asImTexture());
		ImGui::NewLine();
		ImGui::image("Source", screen->settings->sourceTexture->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source int", sourceGrayscale->asImTexture());
		ImGui::SameLine();
		ImGui::arrow({0, Globals::imageWidth / 2}, {Globals::arrowWidth, 0});
	}

	// Histogram
	if (pipeline == 1) {
		ImGui::image("Target hist", targetHistogram->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target int hist", targetGrayscaleHistogram->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Equalized hist", equalizedHistogram->asImTexture());
		ImGui::NewLine();
		ImGui::image("Source hist", sourceHistogram->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source int hist", sourceGrayscaleHistogram->asImTexture());
		ImGui::SameLine();
		ImGui::arrow({0, Globals::imageWidth / 2}, {Globals::arrowWidth, 0});
	}

	// CDF
	if (pipeline == 2) {
		ImGui::image("Target cdf", targetCDF->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target int cdf", targetGrayscaleCDF->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Equalized cdf", equalizedCDF->asImTexture());
		ImGui::NewLine();
		ImGui::image("Source cdf", sourceCDF->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source int cdf", sourceGrayscaleCDF->asImTexture());
		ImGui::SameLine();
		ImGui::arrow({0, Globals::imageWidth / 2}, {Globals::arrowWidth, 0});
	}

	// Edge
	if (pipeline == 3) {
		ImGui::image("Equalized", equalized->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Blur", blur->asImTexture());
		ImGui::SameLine();
		float x = ImGui::GetCursorPosX();

		ImGui::SetCursorPosX(x);
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Sobel X", sobelX->asImTexture());

		ImGui::SetCursorPosX(x);
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Sobel Y", sobelY->asImTexture());

		ImGui::SetCursorPosX(x);
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Sobel XY", sobelXY->asImTexture());

		ImGui::SetCursorPosX(x);
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Canny", canny->asImTexture());

	}
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
	ImageUtils::renderHistogram(sourceGrayscale.get(), sourceGrayscaleHistogram.get());
	ImageUtils::renderHistogram(targetGrayscale.get(), targetGrayscaleHistogram.get());
	ImageUtils::renderCDF(sourceGrayscale.get(), sourceGrayscaleCDF.get());
	ImageUtils::renderCDF(targetGrayscale.get(), targetGrayscaleCDF.get());

	// Calculate equalization
	ImageUtils::renderEqualization(targetGrayscale.get(), sourceGrayscale.get(), equalized.get());

	// Calculate equalization histogram
	ImageUtils::renderHistogram(equalized.get(), equalizedHistogram.get());
	ImageUtils::renderCDF(equalized.get(), equalizedCDF.get());

	// Blur
	ImageUtils::renderBlur(equalized.get(), blur.get());

	// Sobel
	ImageUtils::renderSobel(blur.get(), sobelX.get(), SobelType::X);
	ImageUtils::renderSobel(blur.get(), sobelY.get(), SobelType::Y);
	ImageUtils::renderSobel(blur.get(), sobelXY.get(), SobelType::XY);

	// Canny
	ImageUtils::renderCanny(blur.get(), canny.get());
}
