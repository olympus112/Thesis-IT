#include "core.h"
#include "settings.h"


Settings::Settings() = default;

void Settings::init() {
	actualSourceDimension = Vec2i(1000, 1000);
	preferredTargetDimension = Vec2i(300, 300);
	preferredPatchCountRange = Vec2(4, 100);
	minimumPatchDimension = Vec2i(16, 16);

	rotations = 10;
	sourceRotation = 0.0f;
	imageSize = 350.0;

	sobelDerivative = 1;
	sobelSize = 5;
	sobelType = 0;

	cannyThreshold1 = 100;
	cannyThreshold2 = 200;
	cannyAperture = 3;
	cannyL2gradient = false;

	edgeWeight = 0.5f;
	intensityWeight = 0.5f;
	equalizationWeight = 0.5f;

	std::string sourcePath = "../res/wood_sphere.jpg";
	std::string targetPath = "../res/cliff_sphere.jpg";
	source = RotatedTextures(sourcePath, rotations);
	target = ExtendedTexture("Target", targetPath);
}

void Settings::validateTextureSettings(SettingValidation settingValidation) {
	float targetAspectRatio = target->aspect();
	if (settingValidation == SettingValidation_PreferredTargetDimension) {
		float minimumPatchSurface = minimumPatchDimension.x * minimumPatchDimension.y;
		float minimumTargetSurface = minimumPatchSurface * preferredPatchCountRange.x;
		//float maximumTargetSurface = minimumPatchSurface * preferredPatchCountRange.y;

		// w1 / h1 == w2 / h2
		// aspect == (w2 * h2) / (h2 * h2)
		// aspect == surface / (h2 * h2)
		// h2 = sqrt(surface / aspect)
		float minHeight = std::sqrtf(minimumTargetSurface / targetAspectRatio);
		float minWidth = targetAspectRatio * minHeight;

		//float maxHeight = std::sqrtf(maximumTargetSurface / targetAspectRatio);
		//float maxWidth = targetAspectRatio * maxHeight;

		float currentTargetSurface = preferredTargetDimension.x * preferredTargetDimension.y;

		if (currentTargetSurface < minimumTargetSurface)
			this->preferredTargetDimension = Vec2f(minWidth, minHeight);

		//if (currentTargetSurface > maximumTargetSurface)
		//	this->preferredTargetDimension = Vec2f(maxWidth, maxHeight);

	}
}

float Settings::validateTextureAspect(float* width, float* height, float aspect) {
	if (width == nullptr) {
		return aspect * *height;
	} else if (height == nullptr) {
		return *width / aspect;
	}

	return 0.0f;
}
