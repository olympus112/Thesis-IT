#pragma once

class Patch;
class Mask;

class Match {
public:
	double distance;

	Match(const Patch& patch, const std::vector<cv::Mat>& sourceTextures, const std::vector<cv::Mat>& targetTextures, const std::vector<double>& distribution);
	Match(const Mask& mask, const Vec2& sourceOffset, const Vec2& targetOffset, const std::vector<cv::Mat>& sourceTextures, const std::vector<cv::Mat>& targetTextures, const std::vector<double>& distribution);

	static double computeMatch(const cv::Mat& source, const cv::Mat& target);
};
