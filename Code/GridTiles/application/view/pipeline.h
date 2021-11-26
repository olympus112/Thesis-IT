#pragma once
#include "graphics/texture.h"

class PipelineTab {
public:
	URef<Texture> rgbHistogram;
	URef<Texture> grayscale;
	URef<Texture> grayscaleHistogram;
	URef<Texture> equalized;
	URef<Texture> equalizedHistogram;

	PipelineTab();

	void init();
	void update();
	void render();

	void reload();
};
