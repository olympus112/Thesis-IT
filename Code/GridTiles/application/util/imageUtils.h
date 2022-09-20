#pragma once

#include "main.h"
#include "graphics/textures/texture.h"
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
		Histogram histogram(source->data.clone());
		
		destination->data = histogram.drawLines();
		destination->reloadGL(true);
	}

	inline void renderCDF(Texture* source, Texture* destination) {
		Histogram histogram(source->data.clone());
		CDF cdf(histogram);
		
		destination->data = cdf.draw();
		destination->reloadGL(true);
	}

	inline void renderGrayscale(Texture* source, Texture* destination) {
		Grayscale grayscale(source->data.clone());
		
		destination->data = grayscale.grayscale;
		destination->reloadGL();
	}

	inline void renderEqualization(Texture* source, Texture* reference, Texture* destination) {
		Equalization equalization(source->data.clone(), reference->data.clone());
		
		destination->data = equalization.equalization;
		destination->reloadGL();
	}

	inline void renderBlur(Texture* source, Texture* destination) {
		Blur blur(source->data.clone());
		
		destination->data = blur.blur;
		destination->reloadGL();
	}

	inline void renderSobel(Texture* source, Texture* destination) {
		SobelType sobelTypes[] = {SobelType::X, SobelType::Y, SobelType::XY, SobelType::MAGNITUDE};
		Sobel sobel(source->data.clone(), sobelTypes[settings.sobelType], settings.sobelDerivative, settings.sobelSize);
		
		destination->data = sobel.sobel;
		destination->reloadGL(false);
	}

	inline void renderCanny(Texture* source, Texture* destination) {
		Canny canny(source->data.clone(), settings.cannyThreshold1, settings.cannyThreshold2, settings.cannyAperture, settings.cannyL2gradient);
		
		destination->data = canny.canny;
		destination->reloadGL();
	}

	inline auto saliency = cv::saliency::StaticSaliencyFineGrained::create();
	inline void renderSalience(Texture* source, Texture* destination) {
		cv::Mat result;
		saliency->computeSaliency(source->data.clone(), result);
		result.convertTo(destination->data, CV_8UC1, 255.0);
		destination->reloadGL(false, GL_UNSIGNED_BYTE, GL_LUMINANCE, GL_FLOAT);
	}

}
