#include "core.h"
#include "sourceTexture.h"

SourceTexture::SourceTexture() {
	this->rotations = 0;
}

SourceTexture::SourceTexture(cv::Mat texture, int rotations) {
	this->rotations = rotations;

	cv::Size originalSize(texture.cols, texture.rows);
	cv::Mat originalMask(originalSize, CV_8UC1, cv::Scalar(255));

	for (int rotation = 0; rotation < rotations; rotation++) {
		double angle = 360.0 / rotations * rotation;

		cv::Size rotatedSize = Utils::computeRotatedRect(originalSize, angle).size();
		cv::Mat transformation = Utils::computeTransformationMatrix(originalSize, rotatedSize, angle);

		cv::Mat inverseTransformation;
		cv::invertAffineTransform(transformation, inverseTransformation);

		cv::Mat rotatedMask;
		cv::warpAffine(originalMask, rotatedMask, transformation, rotatedSize);

		cv::Mat rotatedTexture;
		cv::warpAffine(texture, rotatedTexture, transformation, rotatedSize);

		this->masks.push_back(rotatedMask);
		this->textures.push_back(rotatedTexture);
		this->transformations.push_back(transformation);
		this->inverseTransformations.push_back(inverseTransformation);
	}

	reloadTextures();
}

SourceTexture::SourceTexture(cv::Mat texture, const FeatureVector& features, int rotations) {
	this->rotations = rotations;

	cv::Size originalSize(texture.cols, texture.rows);
	cv::Mat originalMask(originalSize, CV_8UC1, cv::Scalar(255));

	for (int rotation = 0; rotation < rotations; rotation++) {
		double angle = 360.0 / rotations * rotation;

		cv::Size rotatedSize = Utils::computeRotatedRect(originalSize, angle).size();
		cv::Mat transformation = Utils::computeTransformationMatrix(originalSize, rotatedSize, angle);

		cv::Mat inverseTransformation;
		cv::invertAffineTransform(transformation, inverseTransformation);

		cv::Mat rotatedMask;
		cv::warpAffine(originalMask, rotatedMask, transformation, rotatedSize);

		cv::Mat rotatedTexture;
		cv::warpAffine(texture, rotatedTexture, transformation, rotatedSize);

		FeatureVector rotatedFeatureVector;
		for (const Texture& feature : features) {
			cv::Mat rotatedFeature;
			cv::warpAffine(feature.data, rotatedFeature, transformation, rotatedSize);

			rotatedFeatureVector.add(rotatedFeature);
		}

		this->masks.emplace_back(rotatedMask);
		this->textures.emplace_back(rotatedTexture);
		this->features.push_back(rotatedFeatureVector);
		this->transformations.push_back(transformation);
		this->transformations.push_back(inverseTransformation);
	}

	reloadTextures();
}

void SourceTexture::setFeatures(const FeatureVector& features) {
	this->features.clear();

	for (int rotationIndex = 0; rotationIndex < rotations; rotationIndex++) {
		cv::Size rotatedSize = cv::Size(masks[rotationIndex].data.cols, masks[rotationIndex].data.rows);
		cv::Mat transformation = transformations[rotationIndex];

		FeatureVector rotatedFeatureVector;
		this->features.push_back(FeatureVector());
		for (const Texture& feature : features) {
			cv::Mat rotatedFeature;
			cv::warpAffine(feature.data, rotatedFeature, transformation, rotatedSize);

			this->features[rotationIndex].add(rotatedFeature);
		}
	}

	reloadTextures();
}

void SourceTexture::reloadTextures() {
	for (int rotationIndex = 0; rotationIndex < rotations; rotationIndex++) {
		textures[rotationIndex].reloadGL();
		masks[rotationIndex].reloadGL();

		if (features.size() > rotationIndex)
			features[rotationIndex].reloadTextures();
	}
}

SourceTexture::SourceTexture(SourceTexture&& other) noexcept {
	this->rotations = other.rotations;  
	this->textures = std::move(other.textures);
	this->features = std::move(other.features);
	this->masks = std::move(other.masks);
	this->transformations = std::move(other.transformations);
	this->inverseTransformations = std::move(other.inverseTransformations);
}

SourceTexture& SourceTexture::operator=(SourceTexture&& other) noexcept {
	this->rotations = other.rotations;
	this->textures = std::move(other.textures);
	this->features = std::move(other.features);
	this->masks = std::move(other.masks);
	this->transformations = std::move(other.transformations);
	this->inverseTransformations = std::move(other.inverseTransformations);

	return *this;
}

Texture* SourceTexture::operator->() {
	return &textures[0];
}

Texture* SourceTexture::operator*() {
	return &textures[0];
}
