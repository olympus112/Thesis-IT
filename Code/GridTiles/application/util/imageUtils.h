#pragma once

#include "graphics/texture.h"
#include "graphics/opencv/blur.h"
#include "graphics/opencv/canny.h"
#include "graphics/opencv/cdf.h"
#include "graphics/opencv/equalization.h"
#include "graphics/opencv/grayscale.h"
#include "graphics/opencv/histogram.h"
#include "graphics/opencv/sobel.h"
#include "opencv2/saliency/saliencySpecializedClasses.hpp"

namespace ImageUtils {

	inline void renderHistogram(Texture* source, Texture* destination) {
		Histogram histogram(source->data);
		
		destination->data = histogram.drawLines();
		destination->reloadGL(true);
	}

	inline void renderCDF(Texture* source, Texture* destination) {
		Histogram histogram(source->data);
		CDF cdf(histogram);
		
		destination->data = cdf.draw();
		destination->reloadGL(true);
	}

	inline void renderGrayscale(Texture* source, Texture* destination) {
		Grayscale grayscale(source->data);
		
		destination->data = grayscale.grayscale;
		destination->reloadGL();
	}

	inline void renderEqualization(Texture* source, Texture* reference, Texture* destination) {
		Equalization equalization(source->data, reference->data);
		
		destination->data = equalization.equalization;
		destination->reloadGL();
	}

	inline void renderBlur(Texture* source, Texture* destination) {
		Blur blur(source->data);
		
		destination->data = blur.blur;
		destination->reloadGL();
	}

	inline void renderSobel(Texture* source, Texture* destination) {
		SobelType sobelTypes[] = {SobelType::X, SobelType::Y, SobelType::XY};
		Sobel sobel(source->data, sobelTypes[screen->settings->sobelType], screen->settings->sobelDerivative, screen->settings->sobelSize);
		
		destination->data = sobel.sobel;
		destination->reloadGL(false);
	}

	inline void renderCanny(Texture* source, Texture* destination) {
		Canny canny(source->data, screen->settings->cannyThreshold1, screen->settings->cannyThreshold2, screen->settings->cannyAperture, screen->settings->cannyL2gradient);
		
		destination->data = canny.canny;
		destination->reloadGL();
	}

	auto saliency = cv::saliency::StaticSaliencyFineGrained::create();
	inline void renderSalience(Texture* source, Texture* destination) {
		saliency->computeSaliency(source->data, destination->data);
		destination->reloadGL(false, GL_RGB32F, GL_LUMINANCE, GL_FLOAT);
	}
}
