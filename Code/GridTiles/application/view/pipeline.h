#pragma once
#include "graphics/textures/extendedTexture.h"
#include "graphics/textures/texture.h"

class PipelineView {
public:
	Texture sourceHistogram;
	Texture sourceCDF;

	ExtendedTexture sourceGrayscale;
	ExtendedTexture targetGrayscale;

	ExtendedTexture equalized;
	ExtendedTexture wequalized;

	Texture targetBlur;
	Texture targetSobel;
	Texture targetCanny;

	Texture sourceBlur;
	Texture sourceSobel;
	Texture sourceCanny;

	Texture saliencyMap;

	PipelineView();

	void init();
	void update();
	void render();

	void reload();
};
