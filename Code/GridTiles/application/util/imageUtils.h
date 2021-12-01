#pragma once
#include <opencv2/imgproc.hpp>

#include "graphics/texture.h"
#include "graphics/opencv/blur.h"
#include "graphics/opencv/canny.h"
#include "graphics/opencv/cdf.h"
#include "graphics/opencv/equalization.h"
#include "graphics/opencv/grayscale.h"
#include "graphics/opencv/histogram.h"
#include "graphics/opencv/sobel.h"

namespace ImageUtils {

	inline void renderHistogram(Texture* source, Texture* destination) {
		Histogram histogram(source->data);
		
		destination->data = histogram.drawLines();
		destination->reloadGL();
	}

	inline void renderCDF(Texture* source, Texture* destination) {
		Histogram histogram(source->data);
		CDF cdf(histogram);
		
		destination->data = cdf.draw();
		destination->reloadGL();
	}

	inline void computeGrayscale(Texture* source, Texture* destination) {
		Grayscale grayscale(source->data);
		
		destination->data = grayscale.grayscale;
		destination->reloadGL(false);
	}

	inline void renderEqualization(Texture* source, Texture* reference, Texture* destination) {
		Equalization equalization(source->data, reference->data);
		
		destination->data = equalization.equalization;
		destination->reloadGL(false);
	}

	inline void renderBlur(Texture* source, Texture* destination) {
		Blur blur(source->data);
		
		destination->data = blur.blur;
		destination->reloadGL(false);
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
		destination->reloadGL(false);
	}
}
