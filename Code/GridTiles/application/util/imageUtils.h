#pragma once
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>

#include "graphics/texture.h"

namespace ImageUtils {

	inline cv::Mat computeHistogram(int size, cv::Mat source, bool rgb = false) {
		int histSize = 256;
		float range[] = {0, 256};
		const float* histRange[] = {range};
		int width = size;
		int height = size;
		cv::Mat result(width, height, CV_8UC3, cv::Scalar(0, 0, 0));
		int bin_w = cvRound(static_cast<double>(width) / histSize);

		if (rgb) {
			std::vector<cv::Mat> bgr_planes;
			cv::split(source, bgr_planes);

			cv::Mat b_hist, g_hist, r_hist;
			calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, histRange, true, false);
			calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, histRange, true, false);
			calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, histRange, true, false);

			normalize(b_hist, b_hist, 0, result.rows, cv::NORM_MINMAX, -1, cv::Mat());
			normalize(g_hist, g_hist, 0, result.rows, cv::NORM_MINMAX, -1, cv::Mat());
			normalize(r_hist, r_hist, 0, result.rows, cv::NORM_MINMAX, -1, cv::Mat());

			for (int i = 1; i < histSize; i++) {
				cv::line(result, cv::Point(bin_w * (i - 1), height - cvRound(b_hist.at<float>(i - 1))),
				         cv::Point(bin_w * (i), height - cvRound(b_hist.at<float>(i))),
				         cv::Scalar(255, 0, 0), 2, 8, 0);
				cv::line(result, cv::Point(bin_w * (i - 1), height - cvRound(g_hist.at<float>(i - 1))),
				         cv::Point(bin_w * (i), height - cvRound(g_hist.at<float>(i))),
				         cv::Scalar(0, 255, 0), 2, 8, 0);
				cv::line(result, cv::Point(bin_w * (i - 1), height - cvRound(r_hist.at<float>(i - 1))),
				         cv::Point(bin_w * (i), height - cvRound(r_hist.at<float>(i))),
				         cv::Scalar(0, 0, 255), 2, 8, 0);
			}

		}
		else {
			cv::Mat histogram;
			cv::calcHist(&source, 1, 0, cv::Mat(), histogram, 1, &histSize, histRange, true, false);

			normalize(histogram, histogram, 0, width, cv::NORM_MINMAX, -1, cv::Mat());
			for (int i = 1; i < histSize; i++) {
				line(result, cv::Point(bin_w * (i - 1), height - cvRound(histogram.at<float>(i - 1))),
				     cv::Point(bin_w * (i), height - cvRound(histogram.at<float>(i))),
				     cv::Scalar(255, 255, 255), 2, 8, 0);
			}

		}

		return result;
	}

	inline void computeHistogram(Texture* source, Texture* destination, bool rgb = true, int size = 500) {
		destination->width = size;
		destination->height = size;
		destination->data = computeHistogram(size, source->data, rgb);
		destination->reloadGL();
	}

	inline void computeGrayscale(Texture* source, Texture* destination) {
		cv::cvtColor(source->data, destination->data, cv::COLOR_BGR2GRAY);
		destination->reloadGL();
	}

	inline void computeEqualization(Texture* source, Texture* target, Texture* destination) {
		cv::equalizeHist(source->data, destination->data);
		destination->reloadGL();
	}
}
