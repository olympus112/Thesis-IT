#include <core.h>
#include "texture.h"

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

Texture::Texture(): Bindable(0) {
}

Texture::Texture(const std::string& path) {
	this->data = cv::imread(path);
	reloadGL();
}

Texture::Texture(const cv::Mat& texture) {
	texture.copyTo(data);
	reloadGL();
}

Texture::~Texture() {
	unbind();
	glDeleteTextures(1, &id);
	this->id = 0;
}

void Texture::setData(int width, int height, const void* data, int internalFormat,
                      unsigned externalFormat, unsigned dataType, unsigned target) {
	if (data) {
		if (this->id == 0)
			this->id = generate(target, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

		bind();

		this->width = width;
		this->height = height;
		this->internalFormat = internalFormat;
		this->externalFormat = externalFormat;
		this->dataType = dataType;
		this->target = target;

		glTexImage2D(target, 0, internalFormat, width, height, 0, externalFormat, dataType, data);
		glGenerateMipmap(target);
	}
	else {
		Log::error("Texture data is null");
	}
}

void Texture::bind() {
	glBindTexture(target, id);
}

void Texture::unbind() {
	glBindTexture(target, 0);
}

void Texture::reloadGL() {
	auto internalFormat = data.channels() == 1 ? GL_LUMINANCE : data.channels() == 3 ? GL_RGB : GL_RGBA;
	auto externalFormat = data.channels() == 1 ? GL_LUMINANCE : data.channels() == 3 ? GL_BGR : GL_BGRA;
	auto dataType = GL_UNSIGNED_BYTE;
	auto target = GL_TEXTURE_2D;

	setData(data.size[0], data.size[1], data.data, internalFormat, externalFormat, dataType, target);
}

GLID Texture::generate(int target, int wrapS, int wrapT, int minFilter, int magFilter) {
	GLID id;

	glGenTextures(1, &id);
	glBindTexture(target, id);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);

	return id;
}

ImTextureID Texture::asImTexture() {
	return reinterpret_cast<ImTextureID>(this->id);
}
