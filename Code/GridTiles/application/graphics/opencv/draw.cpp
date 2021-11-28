#include "core.h"
#include "draw.h"

namespace CV {
static cv::Scalar hsvToBgr(const cv::Scalar& hsv) {
	cv::Mat hsv_mat(1, 1, CV_8UC3, hsv);
	cv::Mat rgb_mat;
	cv::cvtColor(hsv_mat, rgb_mat, cv::COLOR_HSV2BGR);
	return cv::Scalar(rgb_mat.data[0], rgb_mat.data[1], rgb_mat.data[2]);
}

static std::vector<cv::Scalar> getColors(std::size_t n) {
	std::vector<cv::Scalar> colors(n);
	switch (n) {
		case 1:
			colors[0] = cv::Scalar(255, 255, 255);
			break;
		case 3:
			colors[0] = cv::Scalar(255, 0, 0);
			colors[1] = cv::Scalar(0, 255, 0);
			colors[2] = cv::Scalar(0, 0, 255);
			break;
		default:
			for (int i = 0; i < n; i++)
				colors[i] = hsvToBgr(cv::Scalar(180.0 / i, 255, 255));
			break;
	}

	return colors;
}

cv::Mat drawLines(const std::vector<cv::Mat>& lines, int size) {
	cv::Mat result(size, size, CV_8UC3, cv::Scalar(0, 0, 0));

	int nColors = static_cast<int>(lines.size());
	std::vector<cv::Scalar> colors = getColors(nColors);
	for (int line = 0; line < lines.size(); line++) {
		int points = lines[line].rows;
		int binWidth = cvRound(static_cast<double>(size) / points);
		for (int point = 1; point < points; point++) {
			cv::line(
				result,
				cv::Point(binWidth * (point - 1), size - cvRound(lines[line].at<float>(point - 1))),
				cv::Point(binWidth * (point), size - cvRound(lines[line].at<float>(point))),
				colors[line],
				2, 8, 0);
		}
	}

	return result;
}

cv::Mat drawBars(const std::vector<cv::Mat>& bars, int size) {
	const int nColors = static_cast<int>(bars.size());
	const int nBins = bars.front().rows;
	int binWidth = cvRound(static_cast<double>(size) / nBins);

	double local_max;
	double global_max = 0.0;
	for (const cv::Mat& mat : bars) {
		cv::minMaxLoc(mat, 0, &local_max);
		global_max = std::max(global_max, local_max);
	}

	std::cout << "global_max: " << global_max << std::endl;

	std::vector<cv::Scalar> colors = getColors(nColors);

	cv::Mat hist_image(size, binWidth * nColors * nBins, CV_8UC3, cv::Scalar(255, 255, 255));
	for (int color = 0; color < nColors; ++color) {
		for (int x = 0; x < nBins; ++x) {
			const int x1 = (nColors * x + color) * binWidth;
			const int x2 = x1 + binWidth - 1;
			const int y = static_cast<int>((hist_image.rows - 1) * (1.0 - (bars[color].at<float>(x, 0) /
				global_max)));

			cv::rectangle(hist_image, cv::Point(x1, size - 1), cv::Point(x2, y), colors[color], -1);
		}
	}

	return hist_image;
}
}
