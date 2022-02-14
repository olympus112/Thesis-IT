#include <core.h>
#include "SSPGeneration.h"

#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "main.h"

std::unordered_map<SSPGenerationMethod, SRef<SSPG_Method>> sspGenerationMethods {
	{ SSPGenerationMethod_Random, std::make_shared<SSPG_Random>() },
	{ SSPGenerationMethod_TemplateMatch, std::make_shared<SSPG_TemplateMatch>() },
	{ SSPGenerationMethod_SIFT, std::make_shared<SSPG_SIFT>() }
};

//! -------------------------------------------------------------------------------------------------------

void SSPG_Random::renderSettings(Canvas& source, Canvas& target) {
	ImGui::SliderInt("Interdistance##Source", &interdistance, 10, 500);

}

void SSPG_Random::mutateSeedPoints(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) {
	cv::Mat mask(source.texture->data.rows, source.texture->data.cols, CV_8U, cv::Scalar(255));
	for (int i = 0; i < seedPoints.size(); i++) {
		std::vector<cv::Point> nonZero;
		cv::findNonZero(mask, nonZero);
		if (nonZero.empty())
			return;
		cv::Point point = nonZero[cv::theRNG().uniform(0, nonZero.size())];

		cv::circle(mask, point, interdistance, cv::Scalar(0), -1);

		Vec2 sourcePosition = Vec2(point.x, point.y);
		seedPoints[i].sourcePosition = sourcePosition;
	}
}

//! -------------------------------------------------------------------------------------------------------

void SSPG_TemplateMatch::renderSettings(Canvas& source, Canvas& target) {
	ImGui::SliderInt("Interdistance##Source", &interdistance, 10, 500);
	static const char* metrics[] = {
		"SQDIFF",
		"SQDIFF_NORMED",
		"CCORR",
		"CCORR_NORMED",
		"CCOEFF",
		"CCOEFF_NORMED",
	};
	ImGui::Combo("Distance metric", &metric, metrics, 6);
}

void SSPG_TemplateMatch::mutateSeedPoints(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) {
	std::vector<double> distribution = { settings.intensityWeight, settings.edgeWeight };

	int textureRows = source.texture->data.rows;
	int textureCols = source.texture->data.cols;
	cv::Mat mask(textureRows, textureCols, CV_8UC1, cv::Scalar(255));

	for (auto& seedPoint : seedPoints) {
		cv::Rect rect = seedPoint.targetBounds(target.texture).cv();
		int patchRows = rect.height;
		int patchCols = rect.width;

		// Create all features
		std::vector<cv::Mat> features(source.features.size());
		//#pragma omp parallel for
		for (int featureIndex = 0; featureIndex < source.features.size(); featureIndex++) {
			cv::Mat patch(target.features[featureIndex]->data, rect);

			cv::matchTemplate(source.features[featureIndex]->data, patch, features[featureIndex], metric);
			// Todo: check need
			cv::normalize(features[featureIndex], features[featureIndex], 1.0, 0.0, cv::NORM_MINMAX);
			//cv::imshow(std::to_string(featureIndex), features[featureIndex]);
		}

		// Merge features
		int featureRows = textureRows - patchRows + 1;
		int featureCols = textureCols - patchCols + 1;
		cv::Mat output(featureRows, featureCols, CV_32F, cv::Scalar(0.0));
		for (int i = 0; i < features.size(); i++)
			cv::addWeighted(output, 1.0, features[i], distribution[i], 0.0, output);

		//cv::imshow("d", output);

		// Find brightest point
		cv::Point point;
		if (metric == cv::TM_SQDIFF || metric == cv::TM_SQDIFF_NORMED)
			cv::minMaxLoc(output, nullptr, nullptr, &point, nullptr, mask(cv::Rect(0, 0, featureCols, featureRows)));
		else
			cv::minMaxLoc(output, nullptr, nullptr, nullptr, &point, mask(cv::Rect(0, 0, featureCols, featureRows)));

		// Correct point
		cv::Point center(point.x + patchCols / 2, point.y + patchRows / 2);

		// Set mask
		cv::circle(mask, center, interdistance, cv::Scalar(0), -1);

		// Move seedpoint
		seedPoint.sourcePosition = Vec2(center.x, center.y);
	}
}

//! -------------------------------------------------------------------------------------------------------

static cv::Ptr<cv::SIFT> sift = cv::SIFT::create();

void SSPG_SIFT::renderSettings(Canvas& source, Canvas& target) {
	SSPG_Method::renderSettings(source, target);
}

void SSPG_SIFT::mutateSeedPoints(Canvas& source, Canvas& target, std::vector<SeedPoint>& seedPoints) {
	cv::Mat descriptors;
	std::vector<cv::KeyPoint> keyPoints;
	cv::Mat gray = screen.editor.pipeline.targetGrayscaleE->data;
	sift->detectAndCompute(gray, cv::Mat(), keyPoints, descriptors);
	cv::Mat img;
	cv::drawKeypoints(gray, keyPoints, img, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	cv::imshow("SIFT", img);
}
