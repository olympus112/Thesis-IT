#pragma once
#include <GL/glew.h>
#include <opencv2/core/mat.hpp>

#include "bindable.h"
#include "imgui/imgui.h"

class Texture : public Bindable {
public:
	// OpenCV
	cv::Mat data;

	// OpenGL
	int width = 0;
	int height = 0;
	unsigned target = GL_TEXTURE_2D;
	int internalFormat = GL_RGBA;
	unsigned externalFormat = GL_BGR;
	unsigned dataType = GL_UNSIGNED_BYTE;
	int wrapS = GL_REPEAT;
	int wrapT = GL_REPEAT;
	int minFilter = GL_LINEAR_MIPMAP_LINEAR;
	int magFilter = GL_LINEAR;

	Texture();
	Texture(const std::string& path);
	Texture(const cv::Mat& texture);

	~Texture();

	void bind();
	void unbind();
	void reloadGL();

	void setData(int width, int height, const void* data, int internalformat,
	             unsigned externalFormat, unsigned dataType, unsigned target);

	static GLID generate(int target, int wrapS, int wrapT,
	                     int minFilter, int magFilter);

	ImTextureID asImTexture();
};
