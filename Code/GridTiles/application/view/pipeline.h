#pragma once
#include "graphics/textures/extendedTexture.h"
#include "graphics/textures/texture.h"

class PipelineTab {
public:
	URef<Texture> sourceHistogram;
	URef<Texture> sourceCDF;

	URef<Texture> targetHistogram;
	URef<Texture> targetCDF;

	ExtendedTexture sourceGrayscaleE;
	ExtendedTexture targetGrayscaleE;

	ExtendedTexture equalizedE;
	ExtendedTexture wequalizedE;

	URef<Texture> targetBlur;
	URef<Texture> targetSobel;
	URef<Texture> targetCanny;

	URef<Texture> sourceBlur;
	URef<Texture> sourceSobel;
	URef<Texture> sourceCanny;

	URef<Texture> saliencyMap;

	PipelineTab();

	void init();
	void update();
	void render();

	void reload();
	
	void onSourceChanged(bool propagate);
	void onTargetChanged(bool propagate);
	void onSourceOrTargetChanged(bool propagate);
	void onEqualizationWeightChanged(bool propagate);
	void onTargetBlurChanged(bool propagate);
	void onSourceBlurChanged(bool propagate);
};
