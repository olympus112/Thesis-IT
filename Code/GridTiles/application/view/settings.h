#pragma once

#include "graphics/textures/extendedTexture.h"
#include "graphics/textures/rotatedTexture.h"

struct Settings {
	// Array of `rotations` rotated source material textures
	RotatedFeatureTextures source;
	// The target texture
	ExtendedTexture target;

	// Dimension of the source material in millimeters
	Vec2f actualSourceDimension;
	// Dimension of the preferred target puzzle in millimeters
	Vec2f preferredTargetDimension;
	// Minimum dimension of a puzzle patch in millimeters
	Vec2f minimumPatchDimension;
	// Preferred number of patches in the target puzzle
	Vec2i preferredPatchCountRange;

	int rotations;
	float sourceRotation;
	double imageSize;

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

	typedef int SettingValidation;
	enum SettingValidation_ {
		SettingValidation_PreferredTargetDimension
	};

	void validateTextureSettings(SettingValidation settingValidation);
	float validateTextureAspect(float* width, float* height, float aspect);
};
