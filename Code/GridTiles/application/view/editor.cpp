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
	ImGui::Begin("Editor");

	if (ImGui::BeginTabBar("Tabbar")) {
		bool open = true;
		if (ImGui::BeginTabItem("Seedpoints", &open)) {
			seedPointsTab->render();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Pipeline", &open)) {
			pipelineTab->render();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}