#pragma once

#include "texture.h"
#include "graphics/features/Feature.h"

struct RotatedTexture : public Texture {
	double angle = 0.0;
	cv::Mat transformationMatrix;
	cv::Mat inverseTransformationMatrix;

	RotatedTexture();
	RotatedTexture(const std::string& path, double angle);
	RotatedTexture(Texture* texture);
	RotatedTexture(cv::Mat texture);
	RotatedTexture(Texture* texture, double angle);
	RotatedTexture(cv::Mat texture, double angle);

	void resize(const Vec2i& size);

	static cv::Mat transform(cv::Mat texture, cv::Mat transformation, cv::Size size);
	static cv::Mat computeTransformationMatrix(const cv::Size& size, float degrees);
	static cv::Rect computeRotatedRect(const cv::Size& size, float degrees);
	static cv::Rect2f computeRotatedRect2f(const cv::Size& size, float degrees);
};

struct RotatedFeatureTexture : public RotatedTexture {
	FeatureVector features;

	std::size_t size() const;
	bool empty() const;

	Texture& operator[](int index);

	RotatedFeatureTexture();
	RotatedFeatureTexture(const std::string& path, double angle);
	RotatedFeatureTexture(Texture* baseTexture, const FeatureVector& baseFeatures);
	RotatedFeatureTexture(cv::Mat baseTexture, const FeatureVector& baseFeatures);
	RotatedFeatureTexture(Texture* baseTexture, const FeatureVector& baseFeatures, double angle);
	RotatedFeatureTexture(cv::Mat baseTexture, const FeatureVector& baseFeatures, double angle);

	void resize(const Vec2i& size);
};

struct RotatedFeatureTextures {
	std::vector<RotatedFeatureTexture> textures;

	RotatedFeatureTextures();
	RotatedFeatureTextures(RotatedFeatureTextures* textures, int rotations);
	RotatedFeatureTextures(const std::string& path, int rotations);
	RotatedFeatureTextures(Texture* baseTexture, int rotations);
	RotatedFeatureTextures(cv::Mat baseTexture, int rotations);

	RotatedFeatureTextures(RotatedFeatureTextures&& other) noexcept;
	RotatedFeatureTextures& operator=(RotatedFeatureTextures&& other) noexcept;

	RotatedFeatureTextures(const RotatedFeatureTextures& other) = delete;
	RotatedFeatureTextures& operator=(const RotatedFeatureTextures& other) = delete;

	std::size_t size();
	bool empty();

	RotatedFeatureTexture& operator[](int index);
	RotatedFeatureTexture* operator->();
	RotatedFeatureTexture* operator*();

	void resize(const Vec2i& size);
};
