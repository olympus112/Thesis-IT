#pragma once

#include "graphics/imgui/texturepicker.h"

class Settings {
public:
	URef<ImGui::TexturePicker> sourceTexture;
	URef<ImGui::TexturePicker> targetTexture;

	Settings();

	void init();
	void update();
	void render();
};
