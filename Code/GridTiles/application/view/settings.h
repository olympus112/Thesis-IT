#pragma once

#include "graphics/textures/extendedTexture.h"
#include "graphics/textures/rotatedTexture.h"

struct Settings {
	RotatedTextures source;
	ExtendedTexture target;

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
};
