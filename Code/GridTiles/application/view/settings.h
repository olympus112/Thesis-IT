#pragma once

#include "graphics/textures/extendedTexture.h"
#include "graphics/textures/rotatedTexture.h"
#include "graphics/textures/sourceTexture.h"

struct Settings {

	typedef int SettingValidation;
	enum SettingValidation_ {
		SettingValidation_None = 0 << 0,
		SettingValidation_ActualTargetDimension = 1 << 0,
		SettingValidation_ActualSourceDimension = 1 << 1,
		SettingValidation_TargetMillimeterToPixelRatio = 1 << 2,
		SettingValidation_SourceMillimeterToPixelRatio = 1 << 3,
		SettingValidation_SourceToTargetPixelRatio = 1 << 4
	};

	typedef int EdgeMethod;
	enum EdgeMethod_ {
		EdgeMethod_Sobel,
		EdgeMethod_Canny
	};

	// The original non prescaled source image
	Texture originalSource;
	// The original non prescaled target image
	Texture originalTarget;

	// Array of `rotations` rotated source material textures
	SourceTexture sourcer;
	//ExtendedFeatureTexture source;
	// The target texture
	ExtendedFeatureTexture target;
	// The puzzle
	Texture puzzle;

	// The global source mask to indicate what material has been reserved
	Texture mask;

	// Dimension of the source material in millimeters
	Vec2f actualSourceDimension_mm;
	// Scale ratio between source pixels and millimeters
	float sourceMillimeterToPixelRatio;
	// Dimension of the target puzzle in millimeters
	Vec2f actualTargetDimension_mm;
	// Scale ratio between target pixels and millimeters
	float targetMillimeterToPixelRatio;
	// Minimum dimension of a puzzle patch in millimeters
	Vec2f minimumPatchDimension_mm;
	// Preferred number of patches in the target puzzle
	Vec2i preferredPatchCountRange;
	// Scale ratio between source and target pixels
	float sourceToTargetPixelRatio;
	// Postscale factor
	float postscale;
	// Number of rotations
	int rotations;

	int sobelDerivative;
	int sobelSize;
	int sobelType;

	float cannyThreshold1;
	float cannyThreshold2;
	int cannyAperture;
	bool cannyL2gradient;

	EdgeMethod edgeMethod;

	float intensityWeight;
	float edgeWeight;
	float equalizationWeight;

	Settings();

	void init();

	void validateTextureSettings(SettingValidation settingValidation);
	float validateTextureAspect(float* width, float* height, float aspect);
	void reloadPrescaledTextures();

	double spx2mm(double px);
	Vec2 spx2mm(const Vec2& px);
	double tpx2mm(double px);
	Vec2 tpx2mm(const Vec2& px);

	double smm2px(double px);
	Vec2 smm2px(const Vec2& px);
	double tmm2px(double px);
	Vec2 tmm2px(const Vec2& px);
};
