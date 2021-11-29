#pragma once
#include "graphics/texture.h"

class PipelineTab {
public:
	URef<Texture> rotatedSource;
	
	URef<Texture> sourceHistogram;
	URef<Texture> sourceCDF;

	URef<Texture> targetHistogram;
	URef<Texture> targetCDF;

	URef<Texture> sourceGrayscale;
	URef<Texture> sourceGrayscaleHistogram;
	URef<Texture> sourceGrayscaleCDF;

	URef<Texture> targetGrayscale;
	URef<Texture> targetGrayscaleHistogram;
	URef<Texture> targetGrayscaleCDF;

	URef<Texture> equalized;
	URef<Texture> equalizedHistogram;
	URef<Texture> equalizedCDF;

	URef<Texture> wequalized;
	URef<Texture> wequalizedHistogram;
	URef<Texture> wequalizedCDF;

	URef<Texture> blur;
	URef<Texture> sobelX;
	URef<Texture> sobelY;
	URef<Texture> sobelXY;
	URef<Texture> canny;

	PipelineTab();

	void init();
	void update();
	void render();

	void reload();
	
	void onSourceChanged(bool propagate);
	void onTargetChanged(bool propagate);
	void onSourceOrTargetChanged(bool propagate);
	void onSourceRotationChanged(bool propagate);
	void onEqualizationWeightChanged(bool propagate);
};
