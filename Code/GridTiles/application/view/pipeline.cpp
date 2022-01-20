#include <core.h>
#include "pipeline.h"

#include "main.h"
#include "graphics/imgui/widgets.h"
#include "util/imageUtils.h"

static int pipeline = 0;
static std::vector pipelines = {"Default", "Histogram", "CDF", "Blur", "Saliency"};

PipelineTab::PipelineTab() = default;

void PipelineTab::init() {
	sourceHistogram = std::make_unique<Texture>();
	sourceCDF = std::make_unique<Texture>();

	targetHistogram = std::make_unique<Texture>();
	targetCDF = std::make_unique<Texture>();
	
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
		ImGui::image("Target", settings.target.it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target int", targetGrayscaleE->it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Equalized", equalizedE->it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Weighted equalization", wequalizedE->it());
		ImGui::NewLine();
		ImGui::image("Source", settings.source->it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source int", sourceGrayscaleE->it());
		ImGui::SameLine();
		ImGui::arrow({0, Globals::imageWidth / 2}, {Globals::arrowWidth, 0});
	}

	// Histogram
	if (pipeline == 1) {
		ImGui::image("Target hist", targetHistogram->it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target int hist", targetGrayscaleE.histogram.it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Equalized hist", equalizedE.histogram.it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Weighted hist", wequalizedE.histogram.it());
		ImGui::NewLine();
		ImGui::image("Source hist", sourceHistogram->it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source int hist", sourceGrayscaleE.histogram.it());
		ImGui::SameLine();
		ImGui::arrow({0, Globals::imageWidth / 2}, {Globals::arrowWidth, 0});
	}

	// CDF
	if (pipeline == 2) {
		ImGui::image("Target cdf", targetCDF->it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target int cdf", targetGrayscaleE.cdf.it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Equalized cdf", equalizedE.cdf.it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Weighted cdf", wequalizedE.cdf.it());
		ImGui::NewLine();
		ImGui::image("Source cdf", sourceCDF->it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source int cdf", sourceGrayscaleE.cdf.it());
		ImGui::SameLine();
		ImGui::arrow({0, Globals::imageWidth / 2}, {Globals::arrowWidth, 0});
	}

	// Edge
	if (pipeline == 3) {
		ImGui::image("Weight equalized", wequalizedE->it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target blur", targetBlur->it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target Sobel", targetSobel->it());
		ImGui::SameLine();
		ImGui::image("Target Canny", targetCanny->it());

		ImGui::image("Source int", sourceGrayscaleE->it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source blur", sourceBlur->it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source Sobel", sourceSobel->it());
		ImGui::SameLine();
		ImGui::image("Source Canny", sourceCanny->it());
	}

	// Salience
	if (pipeline == 4) {
		ImGui::image("Target", settings.target.it());
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target salience", saliencyMap->it());
	}

	ImGui::End();

}

void PipelineTab::reload() {
	onSourceChanged(false);
	onTargetChanged(false);
	onSourceOrTargetChanged(false);
	onEqualizationWeightChanged(false);
}

void PipelineTab::onSourceChanged(bool propagate) {
	// Source
	ImageUtils::renderHistogram(*settings.source, sourceHistogram.get());
	ImageUtils::renderCDF(*settings.source, sourceCDF.get());

	// Source grayscale
	Grayscale grayscale(*settings.source);
	sourceGrayscaleE = ExtendedTexture("Source grayscale", grayscale.grayscale);

	if (propagate)
		onSourceOrTargetChanged(propagate);
}

void PipelineTab::onTargetChanged(bool propagate) {
	// Target
	ImageUtils::renderHistogram(&settings.target, targetHistogram.get());
	ImageUtils::renderCDF(&settings.target, targetCDF.get());

	// Target grayscale
	Grayscale grayscale(&settings.target);
	targetGrayscaleE = ExtendedTexture("Target grayscale", grayscale.grayscale);

	// Saliency
	ImageUtils::renderSalience(&settings.target, saliencyMap.get());

	if (propagate)
		onSourceOrTargetChanged(propagate);
}

void PipelineTab::onSourceOrTargetChanged(bool propagate) {
	// Calculate equalization
	Equalization equalization(*targetGrayscaleE, *sourceGrayscaleE);
	equalizedE = ExtendedTexture("Equalized", equalization.equalization);

	if (propagate)
		onEqualizationWeightChanged(propagate);
}


void PipelineTab::onEqualizationWeightChanged(bool propagate) {
	// Weighted equalization
	cv::Mat wequalized;
	cv::addWeighted(targetGrayscaleE->data, 1.0f - settings.equalizationWeight, equalizedE->data, settings.equalizationWeight, 0.0, wequalized);
	this->wequalizedE = ExtendedTexture("Weight Equalized", wequalized);

	onTargetBlurChanged(propagate);
}

void PipelineTab::onTargetBlurChanged(bool propagate) {
	// Blur
	ImageUtils::renderBlur(*wequalizedE, targetBlur.get());

	// Sobel
	ImageUtils::renderSobel(targetBlur.get(), targetSobel.get());

	// Canny
	ImageUtils::renderCanny(targetBlur.get(), targetCanny.get());
}

void PipelineTab::onSourceBlurChanged(bool propagate) {
	// Blur
	ImageUtils::renderBlur(*sourceGrayscaleE, sourceBlur.get());

	// Sobel
	ImageUtils::renderSobel(sourceBlur.get(), sourceSobel.get());

	// Canny
	ImageUtils::renderCanny(sourceBlur.get(), sourceCanny.get());
}