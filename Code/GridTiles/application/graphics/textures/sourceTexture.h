#pragma once

#include <vector>
#include "texture.h"
#include "graphics/features/Feature.h"

class SourceTexture {
public:
	int rotations;
	std::vector<Texture> textures;
	std::vector<FeatureVector> features;
	std::vector<Texture> masks;
	std::vector<cv::Mat> transformations;
	std::vector<cv::Mat> inverseTransformations;

	SourceTexture();
	SourceTexture(cv::Mat texture, int rotations);
	SourceTexture(cv::Mat texture, const FeatureVector& features, int rotations);

	~SourceTexture() = default;

	SourceTexture(const SourceTexture& other) = delete;
	SourceTexture(SourceTexture&& other) noexcept;
	SourceTexture& operator=(const SourceTexture& other) = delete;
	SourceTexture& operator=(SourceTexture&& other) noexcept;

	void setFeatures(const FeatureVector& features);

	void reloadTextures();

	Texture* operator->();
	Texture* operator*();
};
