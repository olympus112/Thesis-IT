#pragma once

#include "graphics/imgui/texturepicker.h"

class Settings {
public:
	URef<ImGui::TexturePicker> sourceTexture;
	URef<ImGui::TexturePicker> targetTexture;

	float sourceRotation;

	float patchSize;
	
	float intensityWeight;	
	float edgeWeight;
	float equalizationWeight;

	Settings();

	void init();
	void update();
	void render();
};
