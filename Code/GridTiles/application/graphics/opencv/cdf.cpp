#include "core.h"
#include "cdf.h"

#include "draw.h"

std::vector<cv::Mat> CDF::computeCDF(const std::vector<cv::Mat>& histograms, int nBins, int size) {
	std::vector<cv::Mat> cdf;
	for (int histogram = 0; histogram < histograms.size(); histogram++) {
		cdf.push_back(histograms[histogram].clone());
		for (int i = 1; i < nBins; i++) {
			cdf[histogram].at<float>(i) += cdf[histogram].at<float>(i - 1);
		}

		cv::normalize(cdf[histogram], cdf[histogram], 0, size, cv::NORM_MINMAX, -1, cv::Mat());
	}

	return cdf;
}


CDF::CDF(const std::vector<cv::Mat>& histograms, int nBins) {
	this->nBins = nBins;
	this->nChannels = static_cast<int>(histograms.size());
	this->cdf = computeCDF(histograms, nBins, 512);
}

CDF::CDF(const Histogram& histogram) {
	this->nBins = histogram.nBins;
	this->nChannels = histogram.nChannels;
	this->cdf = computeCDF(histogram.histogram, nBins, 512);
}

cv::Mat CDF::draw() {
	return CV::drawLines(cdf);
}

const cv::Mat& CDF::operator[](int channel) const {
	return cdf.at(channel);
}
