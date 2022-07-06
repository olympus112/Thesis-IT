#include <core.h>
#include "extendedTexture.h"

#include "util/imageUtils.h"

ExtendedTexture::ExtendedTexture(const std::string& name, const std::string& path) {
	this->name = name;
	this->path = path;
	this->texture = Texture(path);

	Histogram histogram(this->texture.data.clone());
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

	Histogram histogram(this->texture.data.clone());
	this->histogram.data = histogram.drawLines();
	this->histogram.reloadGL(true);

	CDF cdf(histogram);
	this->cdf.data = cdf.draw();
	this->cdf.reloadGL(true);
}

ExtendedTexture::ExtendedTexture(const std::string& name, const cv::Mat& texture) {
	this->name = name;
	this->texture = Texture(texture);

	Histogram histogram(this->texture.data.clone());
	this->histogram.data = histogram.drawLines();
	this->histogram.reloadGL(true);

	CDF cdf(histogram);
	this->cdf.data = cdf.draw();
	this->cdf.reloadGL(true);
}

ExtendedTexture::ExtendedTexture(ExtendedTexture&& other) noexcept {
	this->name = other.name;
	this->path = other.path;

	this->texture = std::move(other.texture);
	this->histogram = std::move(other.histogram);
	this->cdf = std::move(other.cdf);
}

ExtendedTexture& ExtendedTexture::operator=(ExtendedTexture&& other) noexcept {
	this->name = other.name;
	this->path = other.path;

	this->texture = std::move(other.texture);
	this->histogram = std::move(other.histogram);
	this->cdf = std::move(other.cdf);

	return *this;
}

void ExtendedTexture::reload() {
	Histogram histogram(this->texture.data.clone());
	this->histogram.data = histogram.drawLines();
	this->histogram.reloadGL(true);

	CDF cdf(histogram);
	this->cdf.data = cdf.draw();
	this->cdf.reloadGL(true);
}

void ExtendedTexture::resize(const Vec2i& size) {
	this->texture.resize(size);
	reload();
}

Texture* ExtendedTexture::operator->() {
	return &texture;
}

Texture* ExtendedTexture::operator*() {
	return &texture;
}

ExtendedFeatureTexture::ExtendedFeatureTexture() = default;

ExtendedFeatureTexture::ExtendedFeatureTexture(ExtendedFeatureTexture&& other) noexcept {
	this->name = other.name;
	this->path = other.path;

	this->features = std::move(other.features);
	this->texture = std::move(other.texture);
	this->histogram = std::move(other.histogram);
	this->cdf = std::move(other.cdf);
}

ExtendedFeatureTexture& ExtendedFeatureTexture::operator=(ExtendedFeatureTexture&& other) noexcept {
	this->name = other.name;
	this->path = other.path;

	this->features = std::move(other.features);
	this->texture = std::move(other.texture);
	this->histogram = std::move(other.histogram);
	this->cdf = std::move(other.cdf);

	return *this;
}

ExtendedFeatureTexture::ExtendedFeatureTexture(const std::string& name, const std::string& path, bool computeFeatures) : ExtendedTexture(name, path) {
	if (computeFeatures)
		this->features = FeatureVector(this->texture.data);
}

ExtendedFeatureTexture::ExtendedFeatureTexture(const std::string& name, Texture* baseTexture, bool computeFeatures) : ExtendedTexture(name, baseTexture) {
	if (computeFeatures)
		this->features = FeatureVector(baseTexture->data);
}

ExtendedFeatureTexture::ExtendedFeatureTexture(const std::string& name, cv::Mat baseTexture, bool computeFeatures) : ExtendedTexture(name, baseTexture) {
	if (computeFeatures)
		this->features = FeatureVector(baseTexture);
}

Texture& ExtendedFeatureTexture::operator[](FeatureIndex feature) {
	return features[feature];
}

void ExtendedFeatureTexture::resize(const Vec2i& size) {
	ExtendedTexture::resize(size);
	features.resize(size);
}
