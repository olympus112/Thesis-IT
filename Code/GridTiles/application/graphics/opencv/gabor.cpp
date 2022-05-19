#include "core.h"
#include "gabor.h"

#include "math/utils.h"

GaborFilter::GaborFilter(double frequency, double theta, double sigmaX, double sigmaY) {
	this->frequency = frequency;
	this->theta = theta;
	this->sigmaX = sigmaX;
	this->sigmaY = sigmaY;

	double cosTheta = std::cos(theta);
	double sinTheta = std::sin(theta);

	double sigmaXSquared = sigmaX * sigmaX;
	double sigmaYSquared = sigmaY * sigmaY;

	double factor = 1.0 / (2.0 * CV_PI * sigmaX * sigmaY);

	int kernelSize = static_cast<int>(CV_PI * std::max(sigmaX, sigmaY));
	this->kernelSizeX = kernelSize;
	this->kernelSizeY = kernelSize;

	/*
	kernel_size_x = static_cast<int>(pi * std::max(sigma_x * std::abs(std::cos(theta)), sigma_y * std::abs(std::sin(theta))));
	kernel_size_y = static_cast<int>(pi * std::max(sigma_x * std::abs(std::sin(theta)), sigma_y * std::abs(std::cos(theta))));
	*/

	this->kernelReal = cv::Mat(2 * kernelSizeY + 1, 2 * kernelSizeX + 1, CV_32FC1);
	this->kernelImaginary = cv::Mat(2 * kernelSizeY + 1, 2 * kernelSizeX + 1, CV_32FC1);

	for (int y = -kernelSizeY; y <= kernelSizeY; ++y) {
		for (int x = -kernelSizeX; x <= kernelSizeX; ++x) {
			const double x_theta = x * cosTheta + y * sinTheta;
			const double y_theta = -x * sinTheta + y * cosTheta;
			const double gaussian = std::exp(-0.5 * (x_theta * x_theta / sigmaXSquared + y_theta * y_theta / sigmaYSquared));

			this->kernelReal.at<float>(y + kernelSizeY, x + kernelSizeX) = static_cast<float>(factor * gaussian * std::cos(
				2.0 * CV_PI * frequency * x_theta));
			this->kernelImaginary.at<float>(y + kernelSizeY, x + kernelSizeX) = static_cast<float>(factor * gaussian * std::sin(
				2.0 * CV_PI * frequency * x_theta));
		}
	}
}

cv::Mat GaborFilter::apply(cv::Mat texture) const {
	cv::Mat response;
	cv::Mat texture_gray;

	if (texture.channels() == 3) {
		cv::cvtColor(texture, texture_gray, cv::COLOR_BGR2GRAY);
	} else {
		texture_gray = texture.clone();
	}

	if (texture_gray.depth() == CV_8U) {
		texture_gray.convertTo(texture_gray, CV_32FC1, 1.0 / 255.0);
	} else if (texture_gray.depth() == CV_16U) {
		texture_gray.convertTo(texture_gray, CV_32FC1, 1.0 / 65535.0);
	}

	cv::Mat response_real, response_imag;
	cv::filter2D(texture_gray, response_real, -1, kernelReal, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT_101);
	cv::filter2D(texture_gray, response_imag, -1, kernelImaginary, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT_101);
	cv::magnitude(response_real, response_imag, response);

	return response;
}

double computeSigmaX(double frequency, double bandwidth_frequency_octaves) {
	const double bandwidthFrequency = std::pow(2.0, bandwidth_frequency_octaves);

	return std::sqrt(std::log(2.0)) * (bandwidthFrequency + 1.0) / (std::sqrt(2.0) * CV_PI * frequency * (bandwidthFrequency - 1.0));
}

double computeSigmaY(double frequency, double bandwidth_angular) {
	return std::sqrt(std::log(2.0)) / (std::sqrt(2.0) * CV_PI * frequency * std::tan(0.5 * bandwidth_angular));
}


GaborFilterBank::GaborFilterBank() : GaborFilterBank(32, 1.0, 4) {}

GaborFilterBank::GaborFilterBank(int resolution, double octaves, int ndirections) {
	std::vector<double> frequencies;
	for (int freq = 4; freq <= resolution / 4; freq *= 2)
		frequencies.push_back((sqrt(2.0) * freq) / resolution);

	this->nfrequencies = static_cast<int>(frequencies.size());
	this->ndirections = ndirections;

	double bandwidth = CV_PI / ndirections;
	std::vector<double> directions;
	for (int i = 0; i < ndirections; i++)
		directions.push_back(bandwidth * i);

	filters.resize(nfrequencies * ndirections);

	for (int i = 0; i < nfrequencies; ++i) {
		double sigmaX = computeSigmaX(frequencies[i], octaves);
		double sigmaY = computeSigmaY(frequencies[i], bandwidth);

		for (int j = 0; j < ndirections; ++j)
			filters[i * ndirections + j] = GaborFilter(frequencies[i], directions[j], sigmaX, sigmaY);
	}
}

std::vector<cv::Mat> GaborFilterBank::compute(cv::Mat texture) const {
	const double gaborMax = 0.2;

	std::vector<cv::Mat> response(filters.size());
	for (int i = 0; i < filters.size(); ++i) {
		response[i] = filters[i].apply(texture);
		response[i] = cv::min(response[i], gaborMax) / gaborMax;
		response[i].convertTo(response[i], CV_16UC1, 65535.0);
	}

	return response;
}

cv::Mat GaborFilterBank::draw() const {
	int max_rows = 0;
	int max_cols = 0;

	for (int i = 0; i < filters.size(); ++i) {
		max_rows = std::max(max_rows, filters[i].kernelReal.rows);
		max_cols = std::max(max_cols, filters[i].kernelReal.cols);
	}

	cv::Mat image = cv::Mat::zeros(height() * max_rows, width() * max_cols, CV_32FC1);

	for (int y = 0; y < height(); ++y) {
		for (int x = 0; x < width(); ++x) {
			cv::Mat kernel;
			cv::normalize(filters[x + width() * y].kernelReal, kernel, 1.0, 0.0, cv::NORM_MINMAX);
			cv::Rect rect_target(x * max_cols, y * max_rows, kernel.cols, kernel.rows);
			kernel.copyTo(image(rect_target));
		}
	}

	return image;
}

int GaborFilterBank::height() const {
	return nfrequencies;
}

int GaborFilterBank::width() const {
	return ndirections;
}

GaborFilter& GaborFilterBank::operator()(int row, int col) {
	return filters[col + row * width()];
}
