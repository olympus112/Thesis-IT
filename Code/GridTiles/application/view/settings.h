#pragma once

#include "graphics/imgui/texturepicker.h"

class Settings {
public:
	URef<ImGui::TexturePicker> sourceTexture;
	URef<ImGui::TexturePicker> targetTexture;

	int rotations;
	float sourceRotation;
	double imageSize;
	int seedPointSize;

	int sobelDerivative;
	int sobelSize;
	int sobelType;

	float cannyThreshold1;
	float cannyThreshold2;
	int cannyAperture;
	bool cannyL2gradient;

	float intensityWeight;	
	float edgeWeight;
	float equalizationWeight;

	Settings();

	void init();
	void update();
	void render();
};
