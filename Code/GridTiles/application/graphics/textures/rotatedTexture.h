#pragma once

#include "texture.h"

struct RotatedTexture : public Texture {
	double angle;
	cv::Mat transformationMatrix;
	cv::Mat inverseTransformationMatrix;

	RotatedTexture();
	RotatedTexture(const std::string& path, double angle);
	RotatedTexture(Texture* texture, double angle);
	RotatedTexture(cv::Mat texture, double angle);
};

struct RotatedTextures {
	std::vector<RotatedTexture> textures;

	RotatedTextures();
	RotatedTextures(const std::string& path, int rotations);

	RotatedTextures(RotatedTextures&& other) noexcept;
	RotatedTextures(const RotatedTextures& other) = delete;
	RotatedTextures& operator=(RotatedTextures&& other) noexcept;
	RotatedTextures& operator=(const RotatedTextures& other) = delete;

	std::size_t size() {
		return textures.size();
	}

	bool empty() {
		return textures.empty();
	}

	RotatedTexture* operator->();
	RotatedTexture* operator*();
};
