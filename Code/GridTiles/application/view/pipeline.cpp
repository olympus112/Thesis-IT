#include <core.h>
#include "pipeline.h"

#include "main.h"
#include "screen.h"
#include "graphics/imgui/widgets.h"
#include "util/imageUtils.h"

static int pipeline = 0;
static std::vector pipelines = {"Default", "Histogram", "CDF", "Blur", "Saliency"};

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
	
	targetBlur = std::make_unique<Texture>();
	targetSobel = std::make_unique<Texture>();
	targetCanny = std::make_unique<Texture>();

	sourceBlur = std::make_unique<Texture>();
	sourceSobel = std::make_unique<Texture>();
	sourceCanny = std::make_unique<Texture>();

	saliencyMap = std::make_unique<Texture>();
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
		ImGui::image("Weight equalized", wequalized->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target blur", targetBlur->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target Sobel", targetSobel->asImTexture());
		ImGui::SameLine();
		ImGui::image("Target Canny", targetCanny->asImTexture());

		ImGui::image("Source int", sourceGrayscale->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source blur", sourceBlur->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source Sobel", sourceSobel->asImTexture());
		ImGui::SameLine();
		ImGui::image("Source Canny", sourceCanny->asImTexture());
	}

	// Salience
	if (pipeline == 4) {
		ImGui::image("Target", screen->settings->targetTexture->texture->asImTexture());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target salience", saliencyMap->asImTexture());
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

	// Grayscale
	ImageUtils::renderGrayscale(rotatedSource.get(), sourceGrayscale.get());

	onSourceBlurChanged(propagate);
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
	ImageUtils::renderGrayscale(screen->settings->targetTexture->texture.get(), targetGrayscale.get());
	ImageUtils::renderHistogram(targetGrayscale.get(), targetGrayscaleHistogram.get());
	ImageUtils::renderCDF(targetGrayscale.get(), targetGrayscaleCDF.get());

	// Saliency
	ImageUtils::renderSalience(screen->settings->targetTexture->texture.get(), saliencyMap.get());

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

	onTargetBlurChanged(propagate);
}

void PipelineTab::onTargetBlurChanged(bool propagate) {
	// Blur
	ImageUtils::renderBlur(wequalized.get(), targetBlur.get());

	// Sobel
	ImageUtils::renderSobel(targetBlur.get(), targetSobel.get());

	// Canny
	ImageUtils::renderCanny(targetBlur.get(), targetCanny.get());
}

void PipelineTab::onSourceBlurChanged(bool propagate) {
	// Blur
	ImageUtils::renderBlur(sourceGrayscale.get(), sourceBlur.get());

	// Sobel
	ImageUtils::renderSobel(sourceBlur.get(), sourceSobel.get());

	// Canny
	ImageUtils::renderCanny(sourceBlur.get(), sourceCanny.get());
}