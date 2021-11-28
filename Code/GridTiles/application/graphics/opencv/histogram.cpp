#include "core.h"
#include "histogram.h"

#include "draw.h"

std::vector<cv::Mat> Histogram::computeHistogram(const std::vector<cv::Mat>& channels,
                                             int nBins, int rangeMin, int rangeMax, int size) {
	float range[] = {rangeMin, rangeMax};
	const float* histogramRange[] = {range};

	std::vector<cv::Mat> histograms(channels.size());
	for (int channel = 0; channel < channels.size(); channel++) {
		cv::calcHist(&channels[channel], 1, nullptr, cv::Mat(), histograms[channel], 1, &nBins, histogramRange, true,
		             false);
		cv::normalize(histograms[channel], histograms[channel], 0, size, cv::NORM_MINMAX, -1, cv::Mat());
	}

	return histograms;
}

Histogram::Histogram(const cv::Mat& texture, int nBins) {
	std::vector<cv::Mat> channels;
	cv::split(texture, channels);

	this->nBins = nBins;
	this->rangeMin = 0;
	this->rangeMax = nBins;
	this->nChannels = static_cast<int>(channels.size());
	this->histogram = computeHistogram(channels, nBins, rangeMin, rangeMax, 512);
}

Histogram::Histogram(const std::vector<cv::Mat>& channels, int nBins) {
	this->nBins = nBins;
	this->rangeMin = 0;
	this->rangeMax = nBins;
	this->nChannels = static_cast<int>(channels.size());
	this->histogram = computeHistogram(channels, nBins, rangeMin, rangeMax, 512);
}

cv::Mat Histogram::drawLines() {
	return CV::drawLines(histogram);
}

cv::Mat Histogram::drawBars() {
	return CV::drawBars(histogram);
}

const cv::Mat& Histogram::operator[](int channel) const {
	return histogram[channel];
}
