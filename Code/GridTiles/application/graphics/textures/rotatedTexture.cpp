#include "core.h"
#include "rotatedTexture.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

static cv::Rect computeRotatedRect(const cv::Mat& texture, double angle) {
	double degrees = 180.0 * angle / CV_PI;
	return cv::RotatedRect(cv::Point2f(), texture.size(), angle).boundingRect();
}

cv::Mat computeTransformationMatrix(const cv::Mat& texture, double angle) {
	const cv::Rect bounds = computeRotatedRect(texture, angle);
	double degrees = 180.0 * angle / CV_PI;
	cv::Point2f center(0.5f * static_cast<float>(texture.cols - 1), 0.5f * static_cast<float>(texture.rows - 1));

	cv::Mat transformationMatrix = cv::getRotationMatrix2D(center, degrees, 1.0);
	transformationMatrix.at<double>(0, 2) += 0.5 * bounds.width - 0.5 * texture.cols;
	transformationMatrix.at<double>(1, 2) += 0.5 * bounds.height - 0.5 * texture.rows;

	return transformationMatrix;
}

RotatedTexture::RotatedTexture() = default;

RotatedTexture::RotatedTexture(const std::string& path, double angle) : RotatedTexture(cv::imread(path), angle) {}

RotatedTexture::RotatedTexture(Texture* texture, double angle) : RotatedTexture(texture->data, angle) {}

RotatedTexture::RotatedTexture(cv::Mat texture, double angle) {
	this->angle = angle;
	this->transformationMatrix = computeTransformationMatrix(texture, angle);
	cv::invertAffineTransform(this->transformationMatrix, this->inverseTransformationMatrix);

	cv::Size bounds = computeRotatedRect(texture, angle).size();
	this->data = transform(texture, this->transformationMatrix, bounds);

	reloadGL(false);
}

cv::Mat RotatedTexture::transform(cv::Mat texture, cv::Mat transformation, cv::Size size) {
	cv::Mat result;
	cv::warpAffine(texture, result, transformation, size);

	return result;
}

//! ---------------------------------------------------------------------------

RotatedFeatureTexture::RotatedFeatureTexture() = default;

RotatedFeatureTexture::RotatedFeatureTexture(const std::string& path, double angle) : RotatedTexture(path, angle) {
	FeatureVector baseFeatures(this->data);
	cv::Size bounds = computeRotatedRect(this->data, angle).size();

	for (const Texture& feature : baseFeatures) {
		cv::Mat rotatedFeature;
		cv::warpAffine(feature.data, rotatedFeature, this->transformationMatrix, bounds);

		this->features.add(rotatedFeature);
	}

	reloadGL();
}

RotatedFeatureTexture::RotatedFeatureTexture(Texture* texture, const FeatureVector& baseFeatures, double angle) : RotatedFeatureTexture(texture->data, baseFeatures, angle) {
	
}

RotatedFeatureTexture::RotatedFeatureTexture(cv::Mat texture, const FeatureVector& baseFeatures, double angle) : RotatedTexture(texture, angle) {
	cv::Size bounds = computeRotatedRect(texture, angle).size();
	for (const Texture& feature : baseFeatures) {
		cv::Mat rotatedFeature;
		cv::warpAffine(feature.data, rotatedFeature, this->transformationMatrix, bounds);

		this->features.add(rotatedFeature);
	}

	reloadGL(false);
}

//! ----------------------------------------------------------------------------------

RotatedFeatureTextures::RotatedFeatureTextures() = default;

RotatedFeatureTextures::RotatedFeatureTextures(const std::string& path, int rotations) {
	cv::Mat texture = cv::imread(path);
	FeatureVector featureVector(texture);
	for (int rotation = 0; rotation < rotations; rotation++) {
		double angle = CV_2PI / rotations * rotation;

		textures.push_back(RotatedFeatureTexture(texture, featureVector, angle));
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
