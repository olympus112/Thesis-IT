#include "core.h"
#include "equalization.h"

std::vector<std::vector<int>> Equalization::computeEqualizationTable(const CDF& source,
	const CDF& reference) {
	std::vector<std::vector<int>> tables;

	int pixel = 0;
	for (int channel = 0; channel < source.nChannels; channel++) {
		tables.push_back(std::vector<int>(source[channel].rows));
		for (int sourcePixel = 0; sourcePixel < source[channel].rows; sourcePixel++) {
			pixel = sourcePixel;
			for (int referencePixel = 0; referencePixel < reference[channel].rows; referencePixel++) {
				if (reference[channel].at<float>(referencePixel) >= source[channel].at<float>(sourcePixel)) {
					pixel = referencePixel;
					break;
				}
			}

			tables[channel][sourcePixel] = pixel;
		}
	}

	return tables;
}

cv::Mat Equalization::computeEqualization(const std::vector<cv::Mat>& channels, std::vector<std::vector<int>> table) {
	std::vector<cv::Mat> output(channels.size());
	for (int channel = 0; channel < channels.size(); channel++) 
		cv::LUT(channels[channel], table[channel], output[channel]);

	cv::Mat result;
	cv::merge(output.data(), output.size(), result);
	cv::convertScaleAbs(result, result);

	return result;
}

Equalization::Equalization(const cv::Mat& texture, const CDF& source, const CDF& reference) {
	std::vector<cv::Mat> channels;
	cv::split(texture, channels);

	this->nChannels = static_cast<int>(channels.size());
	this->table = computeEqualizationTable(source, reference);
	this->equalization = computeEqualization(channels, table);
}

Equalization::Equalization(const cv::Mat& texture, const Histogram& source, const Histogram& reference) {
	std::vector<cv::Mat> channels;
	cv::split(texture, channels);

	CDF sourceCDF(source);
	CDF referenceCDF(reference);

	this->nChannels = sourceCDF.nChannels;
	this->table = computeEqualizationTable(sourceCDF, referenceCDF);
	this->equalization = computeEqualization(channels, table);
}

Equalization::Equalization(const cv::Mat& texture, const cv::Mat& reference) {
	std::vector<cv::Mat> channels;
	cv::split(texture, channels);

	Histogram sourceHistogram(channels);
	Histogram referenceHistogram(reference);

	CDF sourceCDF(sourceHistogram);
	CDF referenceCDF(referenceHistogram);

	this->nChannels = sourceCDF.nChannels;
	this->table = computeEqualizationTable(sourceCDF, referenceCDF);
	this->equalization = computeEqualization(channels, table);
}
