#pragma once
#include "texture.h"

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
	ExtendedTexture(const std::string& name, cv::Mat texture);

	void reload();

	Texture* operator->();
	Texture* operator*();
};
