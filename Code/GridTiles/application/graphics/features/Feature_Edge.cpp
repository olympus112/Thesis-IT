#include <core.h>
#include "Feature_Edge.h"

#include "graphics/opencv/sobel.h"

std::string Feature_Edge::name() {
	return "Edge";
}

cv::Mat Feature_Edge::compute(cv::Mat texture) {
	Sobel sobel(texture, SobelType::XY);

	return sobel.sobel;
}
