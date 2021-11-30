#include "core.h"
#include "settings.h"

#include "main.h"
#include "screen.h"
#include "util/stringUtil.h"

Settings::Settings() = default;

void Settings::init() {
	sourceTexture = std::make_unique<ImGui::TexturePicker>("Source texture");
	targetTexture = std::make_unique<ImGui::TexturePicker>("Target texture");

	sourceRotation = 0.0f;

	patchSize = 5.0f;
	
	edgeWeight = 0.5f;
	intensityWeight = 0.5f;
	equalizationWeight = 0.5f;

	sourceTexture->load("../res/wood_sphere.jpg");
	targetTexture->load("../res/cliff_sphere.jpg");

	screen->editor->pipelineTab->reload();
}

void Settings::update() {
}

void Settings::render() {
	ImGui::Begin("Settings");

	ImGui::Text("Source hover: %s", screen->editor->seedPointsTab->sourceHover ? "Yes" : "No");
	ImGui::Text("Source drag: %s", screen->editor->seedPointsTab->sourceDrag ? "Yes" : "No");
	ImGui::Text("Target hover: %s", screen->editor->seedPointsTab->targetHover ? "Yes" : "No");
	ImGui::Text("Target drag: %s", screen->editor->seedPointsTab->targetDrag ? "Yes" : "No");

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

	if (ImGui::SliderFloat("Source rotation", &sourceRotation, 0.0f, 360.0f))
		screen->editor->pipelineTab->onSourceRotationChanged(true);

	ImGui::SliderFloat("Patch size", &patchSize, 1.0, 50.0);
	
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	
	ImGui::SliderFloat("Intensity weight", &intensityWeight, 0.0f, 1.0f);
	ImGui::SliderFloat("Intensity weight", &edgeWeight, 0.0f, 1.0f);
	if (ImGui::SliderFloat("Equalization weight", &equalizationWeight, 0.0f, 1.0f))
		screen->editor->pipelineTab->onEqualizationWeightChanged(true);
	
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (sourceTexture->render())
		screen->editor->pipelineTab->onSourceChanged(true);

	if (targetTexture->render())
		screen->editor->pipelineTab->onTargetChanged(true);	

	ImGui::End();
}
