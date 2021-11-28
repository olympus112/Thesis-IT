#include "core.h"
#include "settings.h"

#include "main.h"
#include "screen.h"
#include "util/stringUtil.h"

Settings::Settings() = default;

void Settings::init() {
	sourceTexture = std::make_unique<ImGui::TexturePicker>("Source texture");
	targetTexture = std::make_unique<ImGui::TexturePicker>("Target texture");

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

	ImGui::Text("Intersected index: %d", screen->editor->seedPointsTab->intersectedIndex);
	ImGui::Text("Intersected point: %s", Util::str(screen->editor->seedPointsTab->intersectedPoint).c_str());
	ImGui::Text("Selected index: %d", screen->editor->seedPointsTab->intersectedIndex);
	ImGui::Text("Selected point: %s", Util::str(screen->editor->seedPointsTab->selectedPoint).c_str());

	if (sourceTexture->render())
		screen->editor->pipelineTab->reload();

	if (targetTexture->render())
		screen->editor->pipelineTab->reload();	

	ImGui::End();
}
