#include "core.h"
#include "editor.h"

#include "main.h"
#include "screen.h"
#include "imgui/imgui.h"

Editor::Editor() = default;

void Editor::init() {
	pipelineTab = std::make_unique<PipelineTab>();
	seedPointsTab = std::make_unique<SeedPointsTab>();

	pipelineTab->init();
	seedPointsTab->init();
}

void Editor::update() {
	pipelineTab->update();
	seedPointsTab->update();
}

void Editor::render() {
	seedPointsTab->render();
	pipelineTab->render();
}
