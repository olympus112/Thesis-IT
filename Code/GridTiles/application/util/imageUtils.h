#pragma once
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>

#include "graphics/texture.h"

namespace ImageUtils {

	inline std::vector<cv::Mat>	computeHistograms(const std::vector<cv::Mat>& planes, int size = 500, int histogramSize = 256) {
		float range[] = { 0, histogramSize };
		const float* histogramRange[] = { range };

		std::vector<cv::Mat> histograms(planes.size());
		for (int i = 0; i < planes.size(); i++) {
			cv::calcHist(&planes[i], 1, nullptr, cv::Mat(), histograms[i], 1, &histogramSize, histogramRange, true, false);
			cv::normalize(histograms[i], histograms[i], 0, size, cv::NORM_MINMAX, -1, cv::Mat());
		}

		return histograms;
	}

	inline cv::Mat renderHistograms(const std::vector<cv::Mat>& histograms, const std::vector<Color>& colors, int size = 500, int histogramSize = 256) {
		cv::Mat result(size, size, CV_8UC3, cv::Scalar(0, 0, 0));
		int binWidth = cvRound(static_cast<double>(size) / histogramSize);

		for (int i = 1; i < histogramSize; i++) {
			for (int histogram = 0; histogram < histograms.size(); histogram++) {
				cv::line(
					result,
					cv::Point(binWidth * (i - 1), size - cvRound(histograms[histogram].at<float>(i - 1))),
					cv::Point(binWidth * (i), size - cvRound(histograms[histogram].at<float>(i))),
					colors[histogram].cv(), 
					2, 8, 0);
			}
		}

		return result;
	}

	inline std::vector<cv::Mat> computeCDF(const std::vector<cv::Mat>& histograms, int size = 500, int histogramSize = 256) {
		std::vector<cv::Mat> cdf;
		for (int histogram = 0; histogram < histograms.size(); histogram++) {
			cdf.push_back(histograms[histogram].clone());
			for (int i = 1; i < histogramSize; i++) {
				cdf[histogram].at<float>(i) += cdf[histogram].at<float>(i - 1);
			}

			cv::normalize(cdf[histogram], cdf[histogram], 0, size, cv::NORM_MINMAX, -1, cv::Mat());
		}

		return cdf;
	}

	inline void renderHistogram(Texture* source, Texture* destination, bool rgb = true, int size = 500) {
		auto colors = rgb ? std::vector { Colors::RGB_R, Colors::RGB_G, Colors::RGB_B } : std::vector { Colors::WHITE };

		std::vector<cv::Mat> planes;
		cv::split(source->data, planes);

		destination->width = size;
		destination->height = size;
		destination->data = renderHistograms(computeHistograms(planes), colors);
		destination->reloadGL();
	}

	inline void renderCDF(Texture* source, Texture* destination, bool rgb = true, int size = 500) {
		auto colors = rgb ? std::vector { Colors::RGB_R, Colors::RGB_G, Colors::RGB_B } : std::vector { Colors::WHITE };

		std::vector<cv::Mat> planes;
		cv::split(source->data, planes);

		destination->width = size;
		destination->height = size;
		destination->data = renderHistograms(computeCDF(computeHistograms(planes)), colors);
		destination->reloadGL();
	}

	inline void computeGrayscale(Texture* source, Texture* destination) {
		cv::cvtColor(source->data, destination->data, cv::COLOR_BGR2GRAY);
		destination->reloadGL();
	}

	inline std::vector<std::vector<int>> computeEqualizationTables(const std::vector<cv::Mat>& sourceCDFs, const std::vector<cv::Mat>& referenceCDFs) {
		std::vector<std::vector<int>> tables;

		int pixel = 0;
		for (int cdf = 0; cdf < sourceCDFs.size(); cdf++) {
			tables.push_back(std::vector<int>(sourceCDFs[cdf].rows));
			for (int sourcePixel = 0; sourcePixel < sourceCDFs[cdf].rows; sourcePixel++) {
				pixel = sourcePixel;
				for (int referencePixel = 0; referencePixel < referenceCDFs[cdf].rows; referencePixel++) {
					if (referenceCDFs[cdf].at<float>(referencePixel) >= sourceCDFs[cdf].at<float>(sourcePixel)) {
						pixel = referencePixel;
						break;
					}
				}

				tables[cdf][sourcePixel] = pixel;
			}
		}

		return tables;
	}
		
	inline void renderEqualization(Texture* source, Texture* target, Texture* destination) {
		std::vector<cv::Mat> sourcePlanes;
		cv::split(source->data, sourcePlanes);
		auto sourceCDF = computeCDF(computeHistograms(sourcePlanes));

		std::vector<cv::Mat> targetPlanes;
		cv::split(target->data, targetPlanes);
		auto targetCDF = computeCDF(computeHistograms(targetPlanes));

		auto tables = computeEqualizationTables(sourceCDF, targetCDF);

		std::vector<cv::Mat> output(sourcePlanes.size());
		for (int plane = 0; plane < sourcePlanes.size(); plane++) {
			cv::LUT(sourcePlanes[plane], tables[plane], output[plane]);
		}

		cv::merge(output.data(), output.size(), destination->data);
		cv::convertScaleAbs(destination->data, destination->data);
		destination->width = 500;
		destination->height = 500;

		destination->reloadGL();
	}
}
