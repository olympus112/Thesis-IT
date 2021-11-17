#include <core.h>

#include "screen.h"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"

URef<Editor> editor;
URef<Settings> settings;

Screen::Screen() {
	settings = std::make_unique<Settings>();
	editor = std::make_unique<Editor>();
};

void Screen::init() {
	editor->init();
	settings->init();
}

void Screen::update() {
	editor->update();
	settings->update();
}

void Screen::render() {
	ImGui::ShowDemoWindow();

	editor->render();
	settings->render();
}