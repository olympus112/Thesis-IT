#pragma once
#include <GL/glew.h>
#include <opencv2/core/mat.hpp>

#include "../opengl/bindable.h"
#include "imgui/imgui.h"

class Texture : public Bindable {
public:
	// OpenCV
	cv::Mat data;

	// OpenGL
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

	Texture(Texture&& other) noexcept;
	Texture(const Texture& other) = delete;
	Texture& operator=(Texture&& other) noexcept;
	Texture& operator=(const Texture& other) = delete;

	~Texture();

	void bind();
	void unbind();
	void reloadGL(bool linear = false, int internalFormat = 0, int extenalFormat = 0, int dataType = 0);

	void setData(int width,
	             int height,
	             const void* data,
	             int internalformat,
	             unsigned externalFormat,
	             unsigned dataType,
	             unsigned target,
	             bool linear);

	static GLID generate(int target, int wrapS, int wrapT, int minFilter, int magFilter);

	float aspect();
	int cols() const;
	int rows() const;

	ImTextureID it() const;
};
