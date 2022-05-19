#include <core.h>
#include "Feature.h"

#include "Feature_Edge.h"
#include "Feature_Intensity.h"

std::unordered_map<FeatureIndex, SRef<Feature>> Feature::get = {
	{ Feature_Edge::ID, std::make_shared<Feature_Edge>() },
	{ Feature_Intensity::ID, std::make_shared<Feature_Intensity>() }
};

FeatureVector::FeatureVector() = default;

FeatureVector::FeatureVector(Texture* texture) : FeatureVector(texture->data) {
	
}

FeatureVector::FeatureVector(cv::Mat texture) {
	add(Feature::get[FeatureIndex_Intensity]->compute(texture));
	add(Feature::get[FeatureIndex_Edge]->compute(texture));
}

FeatureVector::FeatureVector(const std::vector<cv::Mat>& features) {
	for (cv::Mat feature : features)
		this->features.emplace_back(feature);
}

void FeatureVector::add(const cv::Mat& feature, double weight) {
	if (weight == 0.0)
		features.emplace_back(feature);
	else
		features.emplace_back(feature * weight);
}

int FeatureVector::rows() const {
	return features.front().rows();
}

int FeatureVector::cols() const {
	return features.front().cols();
}

std::size_t FeatureVector::size() const {
	return features.size();
}

bool FeatureVector::empty() const {
	return features.empty();
}

Texture& FeatureVector::operator[](int index) {
	return features[index];
}

const Texture& FeatureVector::operator[](int index) const {
	return features[index];
}


FeatureVector FeatureVector::operator()(const cv::Rect& rect) const {
	FeatureVector featureVector;
	for (const Texture& feature : features)
		featureVector.add(feature.data(rect));

	return featureVector;
}

FeatureVector FeatureVector::operator()(const cv::Range& rowRange, const cv::Range& colRange) const {
	FeatureVector featureVector;
	for (const Texture& feature : features)
		featureVector.add(feature.data(rowRange, colRange));

	return featureVector;
}

std::vector<Texture>::iterator FeatureVector::begin() {
	return features.begin();
}

std::vector<Texture>::iterator FeatureVector::end() {
	return features.end();
}

std::vector<Texture>::const_iterator FeatureVector::begin() const {
	return features.begin();
}

std::vector<Texture>::const_iterator FeatureVector::end() const {
	return features.end();
}

void FeatureVector::reloadTextures() {
	for (Texture& feature : features)
		feature.reloadGL();
}

void FeatureVector::resize(const Vec2i& size) {
	for (Texture& feature : features)
		feature.resize(size);
}
