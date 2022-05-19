#pragma once

// Implementation based on WoodPixel
struct GaborFilter {
public:
	cv::Mat kernelReal;
	cv::Mat kernelImaginary;

	double frequency;
	double theta;
	double sigmaX;
	double sigmaY;
	int kernelSizeX;
	int kernelSizeY;

	GaborFilter() = default;
	GaborFilter(double frequency, double theta, double sigmaX, double sigmaY);

	cv::Mat apply(cv::Mat texture) const;
};

struct GaborFilterBank {
public:
	int nfrequencies;
	int ndirections;
	std::vector<GaborFilter> filters;

	GaborFilterBank();
	GaborFilterBank(int resolution, double octaves, int ndirections);

	std::vector<cv::Mat> compute(cv::Mat texture) const;
	cv::Mat draw() const;

	int height() const;
	int width() const;

	GaborFilter& operator()(int row, int col);
};