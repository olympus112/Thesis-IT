#include <core.h>
#include "texture.h"

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

Texture::Texture(): Bindable(0) {
}

Texture::Texture(const std::string& path) {
	this->data = cv::imread(path);
	reloadGL(false);
}

Texture::Texture(const cv::Mat& texture) {
	texture.copyTo(data);
	reloadGL(false);
}

Texture::Texture(Texture&& other) noexcept {
	this->id = std::exchange(other.id, 0);
	this->data = other.data;
	this->target = other.target;
	this->internalFormat = other.internalFormat;
	this->externalFormat = other.externalFormat;
	this->dataType = other.dataType;
	this->wrapS = other.wrapS;
	this->wrapT = other.wrapT;
	this->minFilter = other.minFilter;
	this->magFilter = other.magFilter;
}

Texture& Texture::operator=(Texture&& other) noexcept {
	this->id = std::exchange(other.id, 0);
	this->data = other.data;
	this->target = other.target;
	this->internalFormat = other.internalFormat;
	this->externalFormat = other.externalFormat;
	this->dataType = other.dataType;
	this->wrapS = other.wrapS;
	this->wrapT = other.wrapT;
	this->minFilter = other.minFilter;
	this->magFilter = other.magFilter;

	return *this;
}

Texture::~Texture() {
	unbind();
	glDeleteTextures(1, &id);
	this->id = 0;
}

void Texture::setData(int width, int height, const void* data, int internalFormat,
                      unsigned externalFormat, unsigned dataType, unsigned target, bool linear) {
	if (data) {
		if (this->id == 0)
			this->id = generate(target, GL_REPEAT, GL_REPEAT, linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST, linear ? GL_LINEAR : GL_NEAREST);

		bind();

		this->internalFormat = internalFormat;
		this->externalFormat = externalFormat;
		this->dataType = dataType;
		this->target = target;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(target, 0, internalFormat, width, height, 0, externalFormat, dataType, data);
		glGenerateMipmap(target);
	} else {
		Log::error("Texture data is null");
	}
}

void Texture::bind() {
	glBindTexture(target, id);
}

void Texture::unbind() {
	glBindTexture(target, 0);
}

void Texture::reloadGL(bool linear, int internalFormat, int extenalFormat, int dataType) {
	auto internalFormat_ = internalFormat != 0 ? internalFormat : data.channels() == 1 ? GL_LUMINANCE : data.channels() == 3 ? GL_RGB : GL_RGBA;
	auto externalFormat_ = extenalFormat != 0 ? extenalFormat : data.channels() == 1 ? GL_LUMINANCE : data.channels() == 3 ? GL_BGR : GL_BGRA;
	auto dataType_ = dataType != 0 ? dataType : GL_UNSIGNED_BYTE;
	auto target = GL_TEXTURE_2D;

	setData(data.size[1], data.size[0], data.data, internalFormat_, externalFormat_, dataType_, target, linear);
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

float Texture::aspect() {
	return static_cast<float>(data.cols) / static_cast<float>(data.rows);
}

ImTextureID Texture::it() const {
	return reinterpret_cast<ImTextureID>(this->id);
}
