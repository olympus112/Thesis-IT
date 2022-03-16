#include "core.h"
#include "settings.h"

#include <opencv2/imgproc.hpp>


Settings::Settings() = default;

void Settings::init() {
	actualSourceDimension_mm = Vec2i(500, 500);
	actualTargetDimension_mm = Vec2i(500, 250);
	preferredPatchCountRange = Vec2(4, 100);
	minimumPatchDimension_mm = Vec2i(10, 10);

	postscale = 0.5;

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

	std::string sourcePath = "../res/voronoi.png";
	std::string targetPath = "../res/eye.jpg";

	originalSource = Texture(sourcePath);
	originalTarget = Texture(targetPath);

	// Validate texture settings againt original source and target
	validateTextureSettings(SettingValidation_ActualSourceDimension | SettingValidation_ActualTargetDimension);

	// Load the prescaled source and target
	reloadPrescaledTextures();
}

void Settings::reloadPrescaledTextures() {
	float sourceSurface = originalSource.surface();
	float targetSurface = originalTarget.surface();

	if (sourceSurface < targetSurface) {
		// Load target and recalculate ratio
		cv::Mat resizedTarget;
		Vec2i newTargetDimension = originalTarget.dimension() * postscale;
		cv::resize(originalTarget.data, resizedTarget, newTargetDimension.cv());
		target = ExtendedFeatureTexture("Target", resizedTarget, false);
		validateTextureSettings(SettingValidation_TargetMillimeterToPixelRatio);

		// Load prescaled source and recalculate ratio
		cv::Mat resizedSource;
		Vec2i newSourceDimension = actualSourceDimension_mm / targetMillimeterToPixelRatio;
		cv::resize(originalSource.data, resizedSource, newSourceDimension.cv());
		source = ExtendedFeatureTexture("Source", resizedSource, false);
		validateTextureSettings(SettingValidation_SourceMillimeterToPixelRatio);

	} else {
		// Load source and recalculate ratio
		cv::Mat resizedSource;
		Vec2i newSourceDimension = originalSource.dimension() * postscale;
		cv::resize(originalSource.data, resizedSource, newSourceDimension.cv());
		source = ExtendedFeatureTexture("Source", resizedSource, false);
		validateTextureSettings(SettingValidation_SourceMillimeterToPixelRatio);

		// Load prescaled target and recalculate ratio
		cv::Mat resizedTarget;
		Vec2i newTargetDimension = actualTargetDimension_mm / sourceMillimeterToPixelRatio;
		cv::resize(originalTarget.data, resizedTarget, newTargetDimension.cv());
		target = ExtendedFeatureTexture("Target", resizedTarget, false);
		validateTextureSettings(SettingValidation_TargetMillimeterToPixelRatio);
	}
	
	// Reset mask
	mask.data = cv::Mat(source->cols(), source->rows(), CV_8UC1, cv::Scalar(255));
	mask.reloadGL();

	// Recalculate source to target ratio, this should be 1
	validateTextureSettings(SettingValidation_SourceToTargetPixelRatio);
}

void Settings::validateTextureSettings(SettingValidation settingValidation) {
	if (settingValidation & SettingValidation_ActualTargetDimension) {
		float targetAspectRatio = originalTarget.aspect();
		float minimumPatchSurface = minimumPatchDimension_mm.x * minimumPatchDimension_mm.y;
		float minimumTargetSurface = minimumPatchSurface * preferredPatchCountRange.x;

		float minHeight = std::sqrtf(minimumTargetSurface / targetAspectRatio);
		float minWidth = targetAspectRatio * minHeight;

		float currentTargetSurface = actualTargetDimension_mm.x * actualTargetDimension_mm.y;

		if (currentTargetSurface < minimumTargetSurface)
			this->actualTargetDimension_mm = Vec2f(minWidth, minHeight);
		else
			this->actualTargetDimension_mm = Vec2f(actualTargetDimension_mm.x, actualTargetDimension_mm.x / targetAspectRatio);
	}

	if (settingValidation & SettingValidation_ActualSourceDimension) {
		float sourceAspectRatio = originalSource.aspect();

		this->actualSourceDimension_mm = Vec2f(actualSourceDimension_mm.x, actualSourceDimension_mm.x / sourceAspectRatio);
	}

	if (settingValidation & SettingValidation_SourceMillimeterToPixelRatio) {
		this->sourceMillimeterToPixelRatio = actualSourceDimension_mm.x / static_cast<float>(source->dimension().x);
	}

	if (settingValidation & SettingValidation_TargetMillimeterToPixelRatio) {
		this->targetMillimeterToPixelRatio = actualTargetDimension_mm.x / static_cast<float>(target->cols());
	}

	if (settingValidation & SettingValidation_SourceToTargetPixelRatio) {
		this->sourceToTargetPixelRatio = targetMillimeterToPixelRatio / sourceMillimeterToPixelRatio;
	}
}

float Settings::validateTextureAspect(float* width, float* height, float aspect) {
	if (width == nullptr) 
		return aspect * *height;

	if (height == nullptr) 
		return *width / aspect;

	return 0.0f;
}

double Settings::spx2mm(double px) {
	return px * sourceMillimeterToPixelRatio;
}

Vec2 Settings::spx2mm(const Vec2& px) {
	return Vec2(spx2mm(px.x), spx2mm(px.y));
}

double Settings::tpx2mm(double px) {
	return px * targetMillimeterToPixelRatio;
}

Vec2 Settings::tpx2mm(const Vec2& px) {
	return Vec2(tpx2mm(px.x), tpx2mm(px.y));
}

double Settings::smm2px(double px) {
	return px / sourceMillimeterToPixelRatio;
}

Vec2 Settings::smm2px(const Vec2& px) {
	return Vec2(smm2px(px.x), smm2px(px.y));
}

double Settings::tmm2px(double px) {
	return px / targetMillimeterToPixelRatio;
}

Vec2 Settings::tmm2px(const Vec2& px) {
	return Vec2(tmm2px(px.x), tmm2px(px.y));
}
