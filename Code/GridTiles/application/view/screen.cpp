#include <core.h>

#include "screen.h"

#include "imgui/imgui.h"

Screen::Screen() = default;

void Screen::init() {
	editor.init();
	settingsView.init();
}

void Screen::update() {
	editor.update();
	settingsView.update();
}

void Screen::render() {
	editor.render();
	settingsView.render();
}