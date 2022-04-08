#include "core.h"
#include "rotatedTexture.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

cv::Rect RotatedTexture::computeRotatedRect(const cv::Size& size, float degrees) {
	return cv::RotatedRect(cv::Point2f(), size, degrees).boundingRect();
}

cv::Rect2f RotatedTexture::computeRotatedRect2f(const cv::Size& size, float degrees) {
	return cv::RotatedRect(cv::Point2f(), size, degrees).boundingRect2f();
}

cv::Mat RotatedTexture::computeTransformationMatrix(const cv::Size& size, float degrees) {
	const cv::Rect bounds = computeRotatedRect(size, degrees);
	cv::Point2f center(0.5f * static_cast<float>(size.width - 1), 0.5f * static_cast<float>(size.height - 1));

	cv::Mat transformationMatrix = cv::getRotationMatrix2D(center, degrees, 1.0);
	transformationMatrix.at<double>(0, 2) += 0.5 * bounds.width - 0.5 * size.width;
	transformationMatrix.at<double>(1, 2) += 0.5 * bounds.height - 0.5 * size.height;

	return transformationMatrix;
}

RotatedTexture::RotatedTexture() = default;

RotatedTexture::RotatedTexture(Texture* texture) : RotatedTexture(texture->data) {}

RotatedTexture::RotatedTexture(cv::Mat texture) {
	this->data = texture;
	this->angle = 0;
	this->transformationMatrix = computeTransformationMatrix(cv::Size(texture.cols, texture.rows), 0);
	cv::invertAffineTransform(this->transformationMatrix, this->inverseTransformationMatrix);
}

RotatedTexture::RotatedTexture(const std::string& path, double angle) : RotatedTexture(cv::imread(path), angle) {}

RotatedTexture::RotatedTexture(Texture* texture, double angle) : RotatedTexture(texture->data, angle) {}

RotatedTexture::RotatedTexture(cv::Mat texture, double angle) {
	this->angle = angle;
	this->transformationMatrix = computeTransformationMatrix(cv::Size(texture.cols, texture.rows), angle);
	cv::invertAffineTransform(this->transformationMatrix, this->inverseTransformationMatrix);

	cv::Size bounds = computeRotatedRect(cv::Size(texture.cols, texture.rows), angle).size();
	this->data = transform(texture, this->transformationMatrix, bounds);

	reloadGL(false);
}

cv::Mat RotatedTexture::transform(cv::Mat texture, cv::Mat transformation, cv::Size size) {
	cv::Mat result;
	cv::warpAffine(texture, result, transformation, size);

	return result;
}

cv::Mat RotatedTexture::rotate(cv::Mat texture, double angle) {
	cv::Mat tranformationMatrix = computeTransformationMatrix(cv::Size(texture.cols, texture.rows), angle);
	cv::Size bounds = computeRotatedRect(cv::Size(texture.cols, texture.rows), angle).size();

	return transform(texture, tranformationMatrix, bounds);
}

void RotatedTexture::resize(const Vec2i& size) {
	Texture::resize(size);
}

std::size_t RotatedFeatureTexture::size() const {
	return features.size();
}

bool RotatedFeatureTexture::empty() const {
	return features.empty();
}

Texture& RotatedFeatureTexture::operator[](int index) {
	return features[index];
}

//! ---------------------------------------------------------------------------

RotatedFeatureTexture::RotatedFeatureTexture() = default;

RotatedFeatureTexture::RotatedFeatureTexture(const std::string& path, double angle) : RotatedTexture(path, angle) {
	FeatureVector baseFeatures(this->data);
	cv::Size bounds = computeRotatedRect(cv::Size(data.cols, data.rows), angle).size();

	for (const Texture& feature : baseFeatures) {
		cv::Mat rotatedFeature;
		cv::warpAffine(feature.data, rotatedFeature, this->transformationMatrix, bounds);

		this->features.add(rotatedFeature);
	}

	reloadGL();
}

RotatedFeatureTexture::RotatedFeatureTexture(Texture* baseTexture, const FeatureVector& baseFeatures) : RotatedFeatureTexture(baseTexture->data, baseFeatures) {

}

RotatedFeatureTexture::RotatedFeatureTexture(cv::Mat baseTexture, const FeatureVector& baseFeatures) : RotatedTexture(baseTexture) {
	for (const Texture& feature : baseFeatures) {
		this->features.add(feature.data);
	}

	reloadGL(false);
}

RotatedFeatureTexture::RotatedFeatureTexture(Texture* texture, const FeatureVector& baseFeatures, double angle) : RotatedFeatureTexture(texture->data, baseFeatures, angle) {
	
}

RotatedFeatureTexture::RotatedFeatureTexture(cv::Mat texture, const FeatureVector& baseFeatures, double angle) : RotatedTexture(texture, angle) {
	cv::Size bounds = computeRotatedRect(cv::Size(data.cols, data.rows), angle).size();
	for (const Texture& feature : baseFeatures) {
		cv::Mat rotatedFeature;
		cv::warpAffine(feature.data, rotatedFeature, this->transformationMatrix, bounds);

		this->features.add(rotatedFeature);
	}

	reloadGL(false);
}

void RotatedFeatureTexture::resize(const Vec2i& size) {
	RotatedTexture::resize(size);
	features.resize(size);
}

//! ----------------------------------------------------------------------------------

RotatedFeatureTextures::RotatedFeatureTextures() = default;

RotatedFeatureTextures::RotatedFeatureTextures(RotatedFeatureTextures* textures, int rotations) {
	RotatedFeatureTexture* baseTexture = &textures->textures[0];

	this->textures.push_back(RotatedFeatureTexture(baseTexture, baseTexture->features));
	for (int rotation = 1; rotation < rotations; rotation++) {
		double angle = CV_2PI / rotations * rotation;

		this->textures.push_back(RotatedFeatureTexture(baseTexture, baseTexture->features, angle));
	}
}

RotatedFeatureTextures::RotatedFeatureTextures(const std::string& path, int rotations) : RotatedFeatureTextures(cv::imread(path), rotations) {

}

RotatedFeatureTextures::RotatedFeatureTextures(Texture* baseTexture, int rotations) : RotatedFeatureTextures(baseTexture->data, rotations) {

}

RotatedFeatureTextures::RotatedFeatureTextures(cv::Mat baseTexture, int rotations) {
	FeatureVector featureVector(baseTexture);

	textures.push_back(RotatedFeatureTexture(baseTexture, featureVector));
	for (int rotation = 1; rotation < rotations; rotation++) {
		double angle = CV_2PI / rotations * rotation;

		textures.push_back(RotatedFeatureTexture(baseTexture, featureVector, angle));
	}
}

RotatedFeatureTextures::RotatedFeatureTextures(RotatedFeatureTextures&& other) noexcept {
	this->textures = std::move(other.textures);
}

RotatedFeatureTextures& RotatedFeatureTextures::operator=(RotatedFeatureTextures&& other) noexcept {
	this->textures = std::move(other.textures);

	return *this;
}

std::size_t RotatedFeatureTextures::size() {
	return textures.size();
}

bool RotatedFeatureTextures::empty() {
	return textures.empty();
}

RotatedFeatureTexture& RotatedFeatureTextures::operator[](int index) {
	return this->textures[index];
}

RotatedFeatureTexture* RotatedFeatureTextures::operator->() {
	if (textures.empty())
		return nullptr;

	return &textures.front();
}

RotatedFeatureTexture* RotatedFeatureTextures::operator*() {
	if (textures.empty())
		return nullptr;

	return &textures.front();
}

void RotatedFeatureTextures::resize(const Vec2i& size) {
	for (RotatedFeatureTexture& texture : textures)
		texture.resize(size);
}
