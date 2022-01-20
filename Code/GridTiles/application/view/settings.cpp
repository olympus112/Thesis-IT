#include "core.h"
#include "settings.h"

Settings::Settings() {
	rotations = 10;
	sourceRotation = 0.0f;
	imageSize = 350.0;
	seedPointSize = 16;

	sobelDerivative = 1;
	sobelSize = 5;
	sobelType = 0;

	cannyThreshold1 = 100;
	cannyThreshold2 = 200;
	cannyAperture = 3;
	cannyL2gradient = false;

	edgeWeight = 0.5f;
	intensityWeight = 0.5f;
	equalizationWeight = 0.5f;
}