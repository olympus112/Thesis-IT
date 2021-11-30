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
	rotatedSource = std::make_unique<Texture>();
	
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

	wequalized = std::make_unique<Texture>();
	wequalizedHistogram = std::make_unique<Texture>();
	wequalizedCDF = std::make_unique<Texture>();
	
	blur = std::make_unique<Texture>();
	sobelX = std::make_unique<Texture>();
	sobelY = std::make_unique<Texture>();
	sobelXY = std::make_unique<Texture>();
	canny = std::make_unique<Texture>();
}

void PipelineTab::update() {
}

void PipelineTab::render() {
	ImGui::Begin("Pipeline");

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
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Weighted equalization", wequalized->asImTexture());
		ImGui::NewLine();
		ImGui::image("Source", rotatedSource->asImTexture());
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
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Weighted hist", wequalizedHistogram->asImTexture());
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
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Weighted cdf", wequalizedCDF->asImTexture());
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
		ImGui::image("Equalized", wequalized->asImTexture());
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

	ImGui::End();

}

void PipelineTab::reload() {
	onSourceChanged(false);
	onTargetChanged(false);
	onSourceOrTargetChanged(false);
	onEqualizationWeightChanged(false);
}

void PipelineTab::onSourceRotationChanged(bool propagate) {
	// Source
	auto size = screen->settings->sourceTexture->texture->data.size;
	cv::Mat rotation = cv::getRotationMatrix2D(cv::Point2f(size[0] / 2.0f, size[1] / 2.0f), screen->settings->sourceRotation, 1.0f);
	cv::warpAffine(screen->settings->sourceTexture->texture->data, rotatedSource->data, rotation, cv::Size(size[0], size[1]));
	rotatedSource->reloadGL();

	ImageUtils::computeGrayscale(rotatedSource.get(), sourceGrayscale.get());
}

void PipelineTab::onSourceChanged(bool propagate) {
	// Source
	onSourceRotationChanged(false);
	
	ImageUtils::renderHistogram(rotatedSource.get(), sourceHistogram.get());
	ImageUtils::renderCDF(rotatedSource.get(), sourceCDF.get());

	// Source grayscale
	ImageUtils::renderHistogram(sourceGrayscale.get(), sourceGrayscaleHistogram.get());
	ImageUtils::renderCDF(sourceGrayscale.get(), sourceGrayscaleCDF.get());

	if (propagate)
		onSourceOrTargetChanged(propagate);
}

void PipelineTab::onTargetChanged(bool propagate) {
	// Target
	ImageUtils::renderHistogram(screen->settings->targetTexture->texture.get(), targetHistogram.get());
	ImageUtils::renderCDF(screen->settings->targetTexture->texture.get(), targetCDF.get());

	// Target grayscale
	ImageUtils::computeGrayscale(screen->settings->targetTexture->texture.get(), targetGrayscale.get());
	ImageUtils::renderHistogram(targetGrayscale.get(), targetGrayscaleHistogram.get());
	ImageUtils::renderCDF(targetGrayscale.get(), targetGrayscaleCDF.get());

	if (propagate)
		onSourceOrTargetChanged(propagate);
}

void PipelineTab::onSourceOrTargetChanged(bool propagate) {
	// Calculate equalization
	ImageUtils::renderEqualization(targetGrayscale.get(), sourceGrayscale.get(), equalized.get());
	ImageUtils::renderHistogram(equalized.get(), equalizedHistogram.get());
	ImageUtils::renderCDF(equalized.get(), equalizedCDF.get());

	if (propagate)
		onEqualizationWeightChanged(propagate);
}


void PipelineTab::onEqualizationWeightChanged(bool propagate) {
	// Weighted equalization
	cv::addWeighted(targetGrayscale->data, 1.0f - screen->settings->equalizationWeight, equalized->data, screen->settings->equalizationWeight, 0.0, wequalized->data);
	wequalized->reloadGL();
	ImageUtils::renderHistogram(wequalized.get(), wequalizedHistogram.get());
	ImageUtils::renderCDF(wequalized.get(), wequalizedCDF.get());

	// Blur
	ImageUtils::renderBlur(wequalized.get(), blur.get());

	// Sobel
	ImageUtils::renderSobel(blur.get(), sobelX.get(), SobelType::X);
	ImageUtils::renderSobel(blur.get(), sobelY.get(), SobelType::Y);
	ImageUtils::renderSobel(blur.get(), sobelXY.get(), SobelType::XY);

	// Canny
	ImageUtils::renderCanny(blur.get(), canny.get());
}
