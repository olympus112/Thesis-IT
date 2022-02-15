#include <core.h>
#include "Feature_Intensity.h"

#include "graphics/opencv/grayscale.h"

std::string Feature_Intensity::name() {
	return "Intensity";
}

cv::Mat Feature_Intensity::compute(cv::Mat texture) {
	Grayscale grayscale(texture);

	return grayscale.grayscale;
}
