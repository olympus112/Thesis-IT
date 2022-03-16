#pragma once

#include <unordered_map>
#include <opencv2/core.hpp>
#include "graphics/textures/texture.h"

typedef int FeatureIndex;
enum FeatureIndex_ {
	FeatureIndex_Intensity,
	FeatureIndex_Edge,
};

struct Feature {
public:
	static std::unordered_map<FeatureIndex, SRef<Feature>> get;

	virtual ~Feature() = default;

	virtual std::string name() = 0;
	virtual cv::Mat compute(cv::Mat texture) = 0;
};

struct FeatureVector {
public:
	std::vector<Texture> features;

	FeatureVector();
	FeatureVector(Texture* texture);
	FeatureVector(cv::Mat texture);
	FeatureVector(const std::vector<cv::Mat>& features);

	void add(const cv::Mat& feature, double weight = 0.0);

	int rows() const;
	int cols() const;

	std::size_t size() const;
	bool empty() const;

	Texture& operator[](int index);

	FeatureVector operator()(const cv::Rect& rect) const;
	FeatureVector operator()(const cv::Range& rowRange, const cv::Range& colRange) const;

	std::vector<Texture>::iterator begin();
	std::vector<Texture>::iterator end();
	std::vector<Texture>::const_iterator begin() const;
	std::vector<Texture>::const_iterator end() const;

	void resize(const Vec2i& size);
};