#include <core.h>
#include "pipeline.h"

#include "main.h"
#include "graphics/imgui/widgets.h"
#include "graphics/textures/sourceTexture.h"
#include "util/imageUtils.h"

static int pipeline = 0;
static std::vector pipelines = {"Default", "Histogram", "CDF", "Blur", "Saliency"};

PipelineView::PipelineView() = default;

void PipelineView::init() {
	reload();
}

void PipelineView::update() {
}

void PipelineView::render() {
	ImGui::Begin("Pipeline");

	for (int index = 0; index < pipelines.size(); index++)
		if (ImGui::RadioButton(pipelines[index], index == pipeline))
			pipeline = index;

	ImGui::Separator();

	ImVec2 sourceSize = ImVec2(Globals::imageWidth, Globals::imageWidth / settings.source->aspect());
	ImVec2 targetSize = ImVec2(Globals::imageWidth, Globals::imageWidth / settings.target->aspect());

	// Default
	if (pipeline == 0) {
		ImGui::image("Target", settings.target->it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target int", targetGrayscale->it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Equalized", equalized->it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Weighted equalization", wequalized->it(), targetSize);
		ImGui::NewLine();
		ImGui::image("Source", settings.source->it(), sourceSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source int", sourceGrayscale->it(), sourceSize);
		ImGui::SameLine();
		ImGui::arrow({0, Globals::imageWidth / 2}, {Globals::arrowWidth, 0});
	}

	// Histogram
	if (pipeline == 1) {
		ImGui::image("Target hist", settings.target.histogram.it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target int hist", targetGrayscale.histogram.it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Equalized hist", equalized.histogram.it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Weighted hist", wequalized.histogram.it(), targetSize);
		ImGui::NewLine();
		ImGui::image("Source hist", sourceHistogram.it(), sourceSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source int hist", sourceGrayscale.histogram.it(), sourceSize);
		ImGui::SameLine();
		ImGui::arrow({0, Globals::imageWidth / 2}, {Globals::arrowWidth, 0});
	}

	// CDF
	if (pipeline == 2) {
		ImGui::image("Target cdf", settings.target.cdf.it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target int cdf", targetGrayscale.cdf.it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Equalized cdf", equalized.cdf.it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Weighted cdf", wequalized.cdf.it(), targetSize);
		ImGui::NewLine();
		ImGui::image("Source cdf", sourceCDF.it(), sourceSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source int cdf", sourceGrayscale.cdf.it(), sourceSize);
		ImGui::SameLine();
		ImGui::arrow({0, Globals::imageWidth / 2}, {Globals::arrowWidth, 0});
	}

	// Edge
	if (pipeline == 3) {
		ImGui::image("Weight equalized", wequalized->it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target blur", targetBlur.it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target Sobel", targetSobel.it(), targetSize);
		ImGui::SameLine();
		ImGui::image("Target Canny", targetCanny.it(), targetSize);

		ImGui::image("Source int", sourceGrayscale->it(), sourceSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source blur", sourceBlur.it(), sourceSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Source Sobel", sourceSobel.it(), sourceSize);
		ImGui::SameLine();
		ImGui::image("Source Canny", sourceCanny.it(), sourceSize);
	}

	// Salience
	if (pipeline == 4) {
		ImGui::image("Target", settings.target->it(), targetSize);
		ImGui::SameLine();
		ImGui::arrow();
		ImGui::SameLine();
		ImGui::image("Target salience", saliencyMap.it(), targetSize);
	}

	ImGui::End();

}

void PipelineView::reload() {
	// Source histogram and cdf
	ImageUtils::renderHistogram(*settings.source, &sourceHistogram);
	ImageUtils::renderCDF(*settings.source, &sourceCDF);

	// Source grayscale
	Grayscale sourceGrayscale(*settings.source);
	this->sourceGrayscale = ExtendedTexture("Source grayscale", sourceGrayscale.grayscale);

	// Target grayscale
	Grayscale targetGrayscale(*settings.target);
	this->targetGrayscale = ExtendedTexture("Target grayscale", targetGrayscale.grayscale);

	// Saliency
	ImageUtils::renderSalience(*settings.target, &saliencyMap);

	// Calculate equalization
	Equalization equalization(*this->targetGrayscale, *this->sourceGrayscale);
	this->equalized = ExtendedTexture("Equalized", equalization.equalization);

	// Weighted equalization
	cv::Mat wequalized;
	cv::addWeighted(this->targetGrayscale->data, 1.0f - settings.equalizationWeight, this->equalized->data, settings.equalizationWeight, 0.0, wequalized);
	this->wequalized = ExtendedTexture("Weight Equalized", wequalized);

	// Target Blur, Sobel and Canny
	ImageUtils::renderBlur(*this->wequalized, &this->targetBlur);
	ImageUtils::renderSobel(&this->targetBlur, &this->targetSobel);
	ImageUtils::renderCanny(&this->targetBlur, &this->targetCanny);

	// Source Blur, Sobel and Canny
	ImageUtils::renderBlur(*this->sourceGrayscale, &this->sourceBlur);
	ImageUtils::renderSobel(&this->sourceBlur, &this->sourceSobel);
	ImageUtils::renderCanny(&this->sourceBlur, &this->sourceCanny);

	// Set source features
	FeatureVector sourceFeatures;
	sourceFeatures.add(this->sourceGrayscale->data.clone());
	sourceFeatures.add(settings.edgeMethod == Settings::EdgeMethod_Sobel ? this->sourceSobel.data.clone() : this->sourceCanny.data.clone());
	settings.sourcer.setFeatures(sourceFeatures);

	// Set source features
	if (settings.source.features.size() == 0) {
		settings.source.features.add(this->sourceGrayscale->data.clone());
		if (settings.edgeMethod == Settings::EdgeMethod_Sobel) {
			settings.source.features.add(this->sourceSobel.data.clone());
		} else if (settings.edgeMethod == Settings::EdgeMethod_Canny) {
			settings.source.features.add(this->sourceCanny.data.clone());
		}
	} else {
		settings.source.features[FeatureIndex_Intensity].data = this->sourceGrayscale->data.clone();
		if (settings.edgeMethod == Settings::EdgeMethod_Sobel)
			settings.source.features[FeatureIndex_Edge].data = sourceSobel.data.clone();
		else if (settings.edgeMethod == Settings::EdgeMethod_Canny)
			settings.source.features[FeatureIndex_Edge].data = sourceCanny.data.clone();
	}

	// Set target features
	if (settings.target.features.size() == 0) {
		settings.target.features.add(this->wequalized->data.clone());
		if (settings.edgeMethod == Settings::EdgeMethod_Sobel)
			settings.target.features.add(this->targetSobel.data.clone());
		else if (settings.edgeMethod == Settings::EdgeMethod_Canny)
			settings.target.features.add(this->targetCanny.data.clone());
	} else {
		settings.target.features[FeatureIndex_Intensity].data = this->wequalized->data.clone();
		if (settings.edgeMethod == Settings::EdgeMethod_Sobel)
			settings.target.features[FeatureIndex_Edge].data = targetCanny.data.clone();
		else if (settings.edgeMethod == Settings::EdgeMethod_Canny)
			settings.target.features[FeatureIndex_Edge].data = targetCanny.data.clone();
	}

	// Reload target and source features
	settings.sourcer.reloadTextures();
	settings.target.features[FeatureIndex_Intensity].reloadGL();
	settings.source.features[FeatureIndex_Intensity].reloadGL();
	settings.target.features[FeatureIndex_Edge].reloadGL();
	settings.source.features[FeatureIndex_Edge].reloadGL();
}