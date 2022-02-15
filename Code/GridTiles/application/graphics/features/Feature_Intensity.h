#pragma once
#include "Feature.h"

struct Feature_Intensity : public Feature {
	inline static FeatureIndex ID = FeatureIndex_Intensity;

	Feature_Intensity() = default;

	std::string name() override;
	cv::Mat compute(cv::Mat texture) override;
};
