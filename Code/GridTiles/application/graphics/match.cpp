#include "core.h"
#include "match.h"

#include "patch.h"

Match::Match(const Patch* patch, const std::vector<cv::Mat>& sourceTextures, const std::vector<cv::Mat>& targetTextures,
             const std::vector<double>& distribution) : Match(patch->mask, patch->sourceOffset, patch->targetOffset,
                                                              sourceTextures, targetTextures, distribution) {}

Match::Match(const Mask& mask, const Vec2& sourceOffset, const Vec2& targetOffset,
             const std::vector<cv::Mat>& sourceTextures,
             const std::vector<cv::Mat>& targetTextures, const std::vector<double>& distribution) {
	distance = 0.0;

	for (std::size_t index = 0; index < sourceTextures.size(); index++) {
		cv::Mat sourcePatch = Mask::copy(sourceTextures[index], mask, sourceOffset.y, sourceOffset.x);
		cv::Mat targetPatch = Mask::copy(targetTextures[index], mask, targetOffset.y, targetOffset.x);

		double normalization = static_cast<double>(sourceTextures[index].size[0] * sourceTextures[index].size[1]) /
			1000.0;
		double increment = distribution[index] * computeMatch(sourcePatch, targetPatch);
		distance += increment / normalization;
	}
}

double Match::computeMatch(const cv::Mat& source, const cv::Mat& target) {
	return cv::norm(source, target);
}
