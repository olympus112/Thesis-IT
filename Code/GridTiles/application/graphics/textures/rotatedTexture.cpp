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

	cv::warpAffine(texture, this->data, this->transformationMatrix, computeRotatedRect(texture, angle).size());

	reloadGL(false);
}

RotatedTextures::RotatedTextures() = default;

RotatedTextures::RotatedTextures(const std::string& path, int rotations) {
	cv::Mat texture = cv::imread(path);
	for (int rotation = 0; rotation < rotations; rotation++) {
		double angle = CV_2PI / rotations * rotation;

		textures.emplace_back(texture, angle);
	}
}

RotatedTextures::RotatedTextures(RotatedTextures&& other) noexcept {
	this->textures = std::move(other.textures);
}

RotatedTextures& RotatedTextures::operator=(RotatedTextures&& other) noexcept {
	this->textures = std::move(other.textures);

	return *this;
}

RotatedTexture* RotatedTextures::operator->() {
	if (textures.empty())
		return nullptr;

	return &textures.front();
}

RotatedTexture* RotatedTextures::operator*() {
	if (textures.empty())
		return nullptr;

	return &textures.front();
}
