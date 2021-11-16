#include <core.h>

#include "screen.h"

#include "imgui/imgui.h"

Screen::Screen() = default;

void Screen::init() {

}

void Screen::update() {

}

void Screen::render() {
	ImGui::Begin("Left");
	ImGui::Text("Hello, left!");
	ImGui::End();

	ImGui::Begin("Down");
	ImGui::Text("Hello, down!");
	ImGui::End();
}
