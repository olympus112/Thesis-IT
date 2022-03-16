#include <core.h>

#include "screen.h"

Screen::Screen() = default;

void Screen::init() {
	pipeline.init();
	editor.init();
	settingsView.init();
}

void Screen::update() {
	pipeline.update();
	editor.update();
	settingsView.update();
}

void Screen::render() {
	pipeline.render();
	editor.render();
	settingsView.render();
}