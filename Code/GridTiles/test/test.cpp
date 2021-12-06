#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "graphics/patch.h"
#include "math/vec.h"
#include <vector>

int main() {
	std::vector<Vec2> points = {
		Vec2(0, 0),
		Vec2(1, 1),
		Vec2(0, 1),
	};

    Patch patch(points);

	cv::imshow("Image", patch.mask);
	cv::waitKey(0);
}
