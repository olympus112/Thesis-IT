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

	std::size_t size() {
		return textures.size();
	}

	bool empty() {
		return textures.empty();
	}

	RotatedTexture* operator->();
	RotatedTexture* operator*();
};
