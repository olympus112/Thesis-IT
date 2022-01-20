#include <core.h>
#include "extendedTexture.h"

#include "util/imageUtils.h"

ExtendedTexture::ExtendedTexture(const std::string& name, const std::string& path) {
	this->name = name;
	this->texture = Texture(path);

	Histogram histogram(this->texture.data);
	this->histogram.data = histogram.drawLines();
	this->histogram.reloadGL(true);

	CDF cdf(histogram);
	this->cdf.data = cdf.draw();
	this->cdf.reloadGL(true);
}

ExtendedTexture::ExtendedTexture(const std::string& name, Texture* texture) {
	this->name = name;
	this->texture.data = texture->data;
	this->texture.reloadGL();

	Histogram histogram(this->texture.data);
	this->histogram.data = histogram.drawLines();
	this->histogram.reloadGL(true);

	CDF cdf(histogram);
	this->cdf.data = cdf.draw();
	this->cdf.reloadGL(true);
}

ExtendedTexture::ExtendedTexture(const std::string& name, cv::Mat texture) {
	this->name = name;
	this->texture = Texture(texture);
	this->texture.reloadGL();

	Histogram histogram(this->texture.data);
	this->histogram.data = histogram.drawLines();
	this->histogram.reloadGL(true);

	CDF cdf(histogram);
	this->cdf.data = cdf.draw();
	this->cdf.reloadGL(true);
}

void ExtendedTexture::reload() {
	Histogram histogram(this->texture.data);
	this->histogram.data = histogram.drawLines();
	this->histogram.reloadGL(true);

	CDF cdf(histogram);
	this->cdf.data = cdf.draw();
	this->cdf.reloadGL(true);
}

Texture* ExtendedTexture::operator->() {
	return &texture;
}

Texture* ExtendedTexture::operator*() {
	return &texture;
}
