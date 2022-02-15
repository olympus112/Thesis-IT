#pragma once

#include "Feature.h"

struct Feature_Edge : public Feature {
	inline static FeatureIndex ID = FeatureIndex_Edge;

	Feature_Edge() = default;

	std::string name() override;
	cv::Mat compute(cv::Mat texture) override;
};
