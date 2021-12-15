#pragma once

#include <opencv2/core.hpp>

class FeatureVector {
public:
	std::vector<cv::Mat> features;

	FeatureVector() = default;
	FeatureVector(const std::vector<cv::Mat>& features) : features(features) {}

	void add(const cv::Mat& feature, double weight = 1.0) {
		if (weight != 1.0)
			features.push_back(feature * weight);
		else
			features.push_back(feature);
	}

	int rows() const {
		return features.front().rows;
	}

	int cols() const {
		return features.front().cols;
	}

	cv::Mat operator[](int index) {
		return features[index];
	}

	FeatureVector operator()(const cv::Rect& rect) const {
		FeatureVector featureVector;
		for (const cv::Mat& feature : features)
			featureVector.add(feature(rect));

		return featureVector;
	}

	FeatureVector operator()(const cv::Range& rowRange, const cv::Range& colRange) const {
		FeatureVector featureVector;
		for (const cv::Mat& feature : features)
			featureVector.add(feature(rowRange, colRange));

		return featureVector;
	}
};