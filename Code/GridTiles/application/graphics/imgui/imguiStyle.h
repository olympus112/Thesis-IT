#pragma once
#include "imgui/imgui.h"

enum ImGuiColors {
	Classic,
	ClassicLight,
	ClassicDark,
	Light,
	Gray,
	Dark
};

extern ImGuiColors theme;
extern ImVec4 text;
extern ImVec4 idle;
extern ImVec4 hover;
extern ImVec4 active;
extern ImVec4 dark;

void setHue(float hue);
void setupImGuiLayoutStyle(ImGuiStyle* style);
void setupImGuiAccent(ImGuiStyle* style);
void setupImGuiColors(ImGuiStyle* style, ImGuiColors colors);
void setupImGuiStyle();
void renderImGuiStyleEditor();