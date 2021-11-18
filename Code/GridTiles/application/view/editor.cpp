#include "core.h"
#include "editor.h"

#include "main.h"
#include "screen.h"
#include "graphics/renderer.h"
#include "graphics/texture.h"
#include "imgui/imgui.h"
#include "util/globals.h"

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "GLFW/glfw3.h"

struct SeedPoint {
	ImVec2 source;
	ImVec2 target;
};

std::vector<SeedPoint> seedPoints;

URef<Texture> rgbHistogram;
URef<Texture> grayscale;
URef<Texture> grayscaleHistogram;
URef<Texture> equalized;
URef<Texture> equalizedHistogram;

Editor::Editor() = default;

cv::Mat computeHistogram(int size, cv::Mat source, bool rgb = false) {
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

void Editor::init() {
	rgbHistogram = std::make_unique<Texture>();
	grayscale = std::make_unique<Texture>();
	grayscaleHistogram = std::make_unique<Texture>();
	equalized = std::make_unique<Texture>();
	equalizedHistogram = std::make_unique<Texture>();
}

void Editor::update() {

}

void Editor::render() {
	ImGui::Begin("Editor");

	if (ImGui::BeginTabBar("Tabbar")) {
		bool open = true;
		if (ImGui::BeginTabItem("Seedpoints", &open)) {
			renderSeedpoints();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Histogram", &open)) {
			renderHistogram();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Editor::reloadSource() {
	// RGB histogram
	rgbHistogram->width = 500;
	rgbHistogram->height = 500;
	rgbHistogram->data = computeHistogram(500, screen->settings->sourceTexture->texture->data, true);
	rgbHistogram->reloadGL();

	// Grayscale
	cv::cvtColor(screen->settings->sourceTexture->texture->data, grayscale->data, cv::COLOR_BGR2GRAY);
	grayscale->reloadGL();

	// Calculate grayscale histogram
	grayscaleHistogram->width = 500;
	grayscaleHistogram->height = 500;
	grayscaleHistogram->data = computeHistogram(500, grayscale->data);
	grayscaleHistogram->reloadGL();

	cv::equalizeHist(grayscale->data, equalized->data);
	equalized->reloadGL();

	equalizedHistogram->width = 500;
	equalizedHistogram->height = 500;
	equalizedHistogram->data = computeHistogram(500, equalized->data);
	equalizedHistogram->reloadGL();
}

void Editor::reloadTarget() {
	cv::equalizeHist(grayscale->data, equalized->data);
	equalized->reloadGL();
}

void Editor::renderHistogram() {
	Render::Image("Source", screen->settings->sourceTexture->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	float cursorX = ImGui::GetCursorPosX();
	Render::Image("Intensity", grayscale->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Equalized", equalized->asImTexture(), Globals::imageSize);
	ImGui::NewLine();
	ImGui::SetCursorPosX(cursorX);
	Render::Image("Target", screen->settings->targetTexture->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow({0, Globals::imageWidth / 2}, {Globals::arrowWidth, 0});

	// Histograms
	Render::Image("RGB hist", rgbHistogram->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Grayscale hist", grayscaleHistogram->asImTexture(), Globals::imageSize);
	ImGui::SameLine();
	Render::arrow();
	ImGui::SameLine();
	Render::Image("Equalized hist", equalizedHistogram->asImTexture(), Globals::imageSize);
}

void Editor::renderSeedpoints() {
	ImVec2 canvasSize = ImVec2(350, 350);

	// Source image
	Render::Image("Source", screen->settings->sourceTexture->asImTexture(), canvasSize);
	ImRect sourceBox(ImGui::GetItemRectMin(), ImGui::GetItemRectMin() + canvasSize);

	ImGui::SameLine();

	// Target image
	Render::Image("Target", screen->settings->targetTexture->asImTexture(), canvasSize);
	ImRect targetBox(ImGui::GetItemRectMin(), ImGui::GetItemRectMin() + canvasSize);
	
	// Hover
	bool sourceHover = false;
	if (ImGui::IsMouseHoveringRect(sourceBox.Min, sourceBox.Max))
		sourceHover = true;
	bool targetHover = false;
	if (ImGui::IsMouseHoveringRect(targetBox.Min, targetBox.Max))
		targetHover = true;

	// Bounding boxes
	ImGui::GetWindowDrawList()->AddRect(sourceBox.Min, sourceBox.Max, Colors::WHITE.u32(), 0, ImDrawCornerFlags_All, 2);
	ImGui::GetWindowDrawList()->AddRect(targetBox.Min, targetBox.Max, Colors::WHITE.u32(), 0, ImDrawCornerFlags_All, 2);

	// Cursor
	if (sourceHover || targetHover) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		float size = 7;
		float thickness = 3;
		ImVec2 cursor = ImGui::GetMousePos();
		ImGui::GetWindowDrawList()->AddLine(cursor - ImVec2(size, 0), cursor + ImVec2(size, 0), Colors::WHITE.u32(), thickness);
		ImGui::GetWindowDrawList()->AddLine(cursor - ImVec2(0, size), cursor + ImVec2(0, size), Colors::WHITE.u32(), thickness);

		if (sourceHover) {
			ImGui::GetWindowDrawList()->AddLine(ImVec2(sourceBox.Min.x, cursor.y), ImVec2(sourceBox.Min.x + size, cursor.y), Colors::WHITE.u32(), thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(sourceBox.Max.x, cursor.y), ImVec2(sourceBox.Max.x - size, cursor.y), Colors::WHITE.u32(), thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, sourceBox.Min.y), ImVec2(cursor.x, sourceBox.Min.y + size), Colors::WHITE.u32(), thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, sourceBox.Max.y), ImVec2(cursor.x, sourceBox.Max.y - size), Colors::WHITE.u32(), thickness);
		}

		if (targetHover) {
			ImGui::GetWindowDrawList()->AddLine(ImVec2(targetBox.Min.x, cursor.y), ImVec2(targetBox.Min.x + size, cursor.y), Colors::WHITE.u32(), thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(targetBox.Max.x, cursor.y), ImVec2(targetBox.Max.x - size, cursor.y), Colors::WHITE.u32(), thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, targetBox.Min.y), ImVec2(cursor.x, targetBox.Min.y + size), Colors::WHITE.u32(), thickness);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, targetBox.Max.y), ImVec2(cursor.x, targetBox.Max.y - size), Colors::WHITE.u32(), thickness);
		}
	}

	// Place seedPoints
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && (sourceHover || targetHover)) {
		ImVec2 offset = ImGui::GetMousePos() - (sourceHover ? sourceBox : targetBox).Min;
		seedPoints.push_back({
			offset,
			offset
		});
	}

	// Render seedPoints
	for (const auto& seedPoint : seedPoints) {
		ImVec2 size(5, 5);
		ImGui::GetWindowDrawList()->AddRectFilled(sourceBox.Min + seedPoint.source - size, sourceBox.Min + seedPoint.source + size, Colors::RED.u32(), 0, ImDrawCornerFlags_All);
		ImGui::GetWindowDrawList()->AddRectFilled(targetBox.Min + seedPoint.target - size, targetBox.Min + seedPoint.target + size, Colors::RED.u32(), 0, ImDrawCornerFlags_All);
	}
}
