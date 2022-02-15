#pragma once

#include <opencv2/imgcodecs.hpp>

#include "texture.h"
#include "graphics/features/Feature.h"

struct RotatedTexture : public Texture {
	double angle;
	cv::Mat transformationMatrix;
	cv::Mat inverseTransformationMatrix;

	RotatedTexture();
	RotatedTexture(const std::string& path, double angle);
	RotatedTexture(Texture* texture, double angle);
	RotatedTexture(cv::Mat texture, double angle);

	static cv::Mat transform(cv::Mat texture, cv::Mat transformation, cv::Size size);
};

struct RotatedFeatureTexture : public RotatedTexture {
	FeatureVector features;

	RotatedFeatureTexture();
	RotatedFeatureTexture(const std::string& path, double angle);
	RotatedFeatureTexture(Texture* baseTexture, const FeatureVector& baseFeatures, double angle);
	RotatedFeatureTexture(cv::Mat baseTexture, const FeatureVector& baseFeatures, double angle);
};

struct RotatedFeatureTextures {
	std::vector<RotatedFeatureTexture> textures;

	RotatedFeatureTextures();
	RotatedFeatureTextures(const std::string& path, int rotations);

	RotatedFeatureTextures(RotatedFeatureTextures&& other) noexcept;
	RotatedFeatureTextures& operator=(RotatedFeatureTextures&& other) noexcept;

	RotatedFeatureTextures(const RotatedFeatureTextures& other) = delete;
	RotatedFeatureTextures& operator=(const RotatedFeatureTextures& other) = delete;

	std::size_t size();
	bool empty();

	RotatedFeatureTexture& operator[](int index);
	RotatedFeatureTexture* operator->();
	RotatedFeatureTexture* operator*();
};
