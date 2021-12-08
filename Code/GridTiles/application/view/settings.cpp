#include "core.h"
#include "settings.h"

#include "main.h"
#include "screen.h"
#include "graphics/imgui/widgets.h"
#include "util/stringUtil.h"

Settings::Settings() = default;

void Settings::init() {
	sourceTexture = std::make_unique<ImGui::TexturePicker>("Source texture");
	targetTexture = std::make_unique<ImGui::TexturePicker>("Target texture");

	rotations = 10;
	sourceRotation = 0.0f;
	imageSize = 350.0;

	sobelDerivative = 1;
	sobelSize = 5;
	sobelType = 0;

	cannyThreshold1 = 100;
	cannyThreshold2 = 200;
	cannyAperture = 3;
	cannyL2gradient = false;

	edgeWeight = 0.5f;
	intensityWeight = 0.5f;
	equalizationWeight = 0.5f;

	sourceTexture->load("../res/wood_sphere.jpg");
	targetTexture->load("../res/cliff_sphere.jpg");

	screen->editor->pipelineTab->reload();
	screen->editor->seedPointsTab->reload();
}

void Settings::update() {
}

void Settings::render() {
	ImGui::Begin("Settings");

	ImGui::Text("Source hover: %s", screen->editor->seedPointsTab->source.hover ? "Yes" : "No");
	ImGui::Text("Source drag: %s", screen->editor->seedPointsTab->source.drag ? "Yes" : "No");
	ImGui::Text("Target hover: %s", screen->editor->seedPointsTab->target.hover ? "Yes" : "No");
	ImGui::Text("Target drag: %s", screen->editor->seedPointsTab->target.drag ? "Yes" : "No");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Intersected index: %d", screen->editor->seedPointsTab->intersectedIndex);
	ImGui::Text("Intersected point: %s", Util::str(screen->editor->seedPointsTab->intersectedPoint).c_str());
	ImGui::Text("Selected index: %d", screen->editor->seedPointsTab->intersectedIndex);
	ImGui::Text("Selected point: %s", Util::str(screen->editor->seedPointsTab->selectedPoint).c_str());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::SliderInt("Rotations", &rotations, 1, 30)) {
		float interval = 360.0f / rotations;
		float distanceFromFloor = fmod(sourceRotation, interval);
		sourceRotation -= distanceFromFloor < interval / 2.0f ? distanceFromFloor : -(interval - distanceFromFloor);
		screen->editor->pipelineTab->onSourceRotationChanged(true);
		//screen->editor->seedPointsTab->onRecalculateMatching();
	}

	if (ImGui::SliderFloatWithSteps("Source rotation", &sourceRotation, 0.0f, 360.0f, 360.0f / rotations, "%.2f")) {
		screen->editor->pipelineTab->onSourceRotationChanged(true);
		//screen->editor->seedPointsTab->onRecalculateMatching();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	
	if (ImGui::SliderFloat("Intensity weight", &intensityWeight, 0.0f, 1.0f)) {
		//screen->editor->seedPointsTab->onRecalculateMatching();
		edgeWeight = 1.0f - intensityWeight;
	}
	if (ImGui::SliderFloat("Edge weight", &edgeWeight, 0.0f, 1.0f)) {
		//screen->editor->seedPointsTab->onRecalculateMatching();
		intensityWeight = 1.0f - edgeWeight;
	}
	if (ImGui::SliderFloat("Equalization weight", &equalizationWeight, 0.0f, 1.0f)) {
		screen->editor->pipelineTab->onEqualizationWeightChanged(true);
		//screen->editor->seedPointsTab->onRecalculateMatching();
	}
	
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	bool blurChanged = false;
	blurChanged |= ImGui::RadioButton("X", &sobelType, 0);
	ImGui::SameLine();
	blurChanged |= ImGui::RadioButton("Y", &sobelType, 1);
	ImGui::SameLine();
	blurChanged |= ImGui::RadioButton("XY", &sobelType, 2);
	ImGui::SameLine();
	ImGui::Text("Sobel type");

	blurChanged |= ImGui::RadioButton("1##sobel", &sobelSize, 1);
	ImGui::SameLine();
	blurChanged |= ImGui::RadioButton("3##sobel", &sobelSize, 3);
	ImGui::SameLine();
	blurChanged |= ImGui::RadioButton("5##sobel", &sobelSize, 5);
	ImGui::SameLine();
	blurChanged |= ImGui::RadioButton("7##sobel", &sobelSize, 7);
	ImGui::SameLine();
	ImGui::Text("Sobel size");
	blurChanged |= ImGui::SliderInt("Sobel derivative", &sobelDerivative, 1, 5);

	ImGui::Separator();

	blurChanged |= ImGui::SliderFloat("Canny T1", &cannyThreshold1, 0, cannyThreshold2);
	blurChanged |= ImGui::SliderFloat("Canny T2", &cannyThreshold2, cannyThreshold1, 1000);
	blurChanged |= ImGui::RadioButton("3##canny", &cannyAperture, 3);
	ImGui::SameLine();
	blurChanged |= ImGui::RadioButton("5##canny", &cannyAperture, 5);
	ImGui::SameLine();
	blurChanged |= ImGui::RadioButton("7##canny", &cannyAperture, 7);
	ImGui::SameLine();
	ImGui::Text("Canny aperture");
	blurChanged |= ImGui::Checkbox("Canny L2 gradient", &cannyL2gradient);

	if (blurChanged) {
		screen->editor->pipelineTab->onTargetBlurChanged(true);
		screen->editor->pipelineTab->onSourceBlurChanged(true);
		//screen->editor->seedPointsTab->onRecalculateMatching();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (targetTexture->render()) {
		screen->editor->pipelineTab->onTargetChanged(true);
		screen->editor->seedPointsTab->onTargetChanged();
	}

	if (sourceTexture->render()) {
		screen->editor->pipelineTab->onSourceChanged(true);
		screen->editor->seedPointsTab->onSourceChanged();
	}

	ImGui::End();
}
