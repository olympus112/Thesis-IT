#pragma once
#include "texture.h"
#include "graphics/features/Feature.h"

class ExtendedTexture {
public:
	std::string name;
	std::string path;

	Texture texture;
	Texture histogram;
	Texture cdf;

	ExtendedTexture() = default;
	ExtendedTexture(const std::string& name, const std::string& path);
	ExtendedTexture(const std::string& name, Texture* texture);
	ExtendedTexture(const std::string& name, const cv::Mat& texture);

	ExtendedTexture(const ExtendedTexture& other) = delete;
	ExtendedTexture(ExtendedTexture&& other) noexcept;
	ExtendedTexture& operator=(const ExtendedTexture& other) = delete;
	ExtendedTexture& operator=(ExtendedTexture&& other) noexcept;

	~ExtendedTexture() {
		if (texture.id != 0)
			Log::debug("Deleted %s [%s] %d %d %d", name.c_str(), path.c_str(), texture.id, histogram.id, cdf.id);
	}

	void reload();
	void resize(const Vec2i& size);

	Texture* operator->();
	Texture* operator*();
};

class ExtendedFeatureTexture : public ExtendedTexture {
public:
	FeatureVector features;

	ExtendedFeatureTexture();
	ExtendedFeatureTexture& operator=(ExtendedFeatureTexture&& other) noexcept;
	ExtendedFeatureTexture(ExtendedFeatureTexture&& other) noexcept;

	ExtendedFeatureTexture(const std::string& name, const std::string& path, bool computeFeatures);
	ExtendedFeatureTexture(const std::string& name, Texture* baseTexture, bool computeFeatures);
	ExtendedFeatureTexture(const std::string& name, cv::Mat baseTexture, bool computeFeatures);

	Texture& operator[](FeatureIndex feature);

	void resize(const Vec2i& size);
};