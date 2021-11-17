#include "core.h"
#include "settings.h"

#include "main.h"
#include "screen.h"

Settings::Settings() = default;

void Settings::init() {
	sourceTexture = std::make_unique<ImGui::TexturePicker>("Source texture");
	targetTexture = std::make_unique<ImGui::TexturePicker>("Target texture");

	sourceTexture->load("../res/uv.jpg");
	screen->editor->reloadSource();

	targetTexture->load("../res/uv.jpg");
	screen->editor->reloadTarget();
}

void Settings::update() {
}

void Settings::render() {
	ImGui::Begin("Settings");

	if (sourceTexture->render())
		screen->editor->reloadSource();

	if (targetTexture->render())
		screen->editor->reloadTarget();
	

	ImGui::End();
}
