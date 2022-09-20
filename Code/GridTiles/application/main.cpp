#include "core.h"
#include "main.h"

#include <algorithm>
#include <opencv2/highgui.hpp>

#include "graphics/imgui/imguiStyle.h"
#include "graphics/opencv/equalization.h"
#include "graphics/opencv/gabor.h"
#include "graphics/opencv/grayscale.h"
#include "graphics/opencv/sobel.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "opencv2/saliency/saliencyBaseClasses.hpp"
#include "rolling_guidance/RollingGuidanceFilter.h"
#include "util/imageUtils.h"

std::mutex MUTEX_RENDER;
Vec2i dimension;
Screen screen;
GLFWwindow* window;
Settings settings;

bool init() {
	// GLFW init
	if (!glfwInit()) {
		Log::error("GLFW init failed");

		return false;
	}

	// Window
	window = glfwCreateWindow(1920, 1080, "Grid Tiles", /*glfwGetPrimaryMonitor()*/ nullptr, nullptr);
	if (!window) {
		Log::error("GLFW window creation failed");
		glfwTerminate();

		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetErrorCallback([](int code, const char* error) {
		Log::error("%d: %s", code, error);
	});

	if (GLenum error = glewInit(); error != GLEW_OK) {
		Log::error("%s", glewGetErrorString(error));
		glfwTerminate();

		return false;
	}

	// Opengl error
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback([](GLenum source,
	                          GLenum type,
	                          GLuint id,
	                          GLenum severity,
	                          GLsizei length,
	                          const GLchar* message,
	                          const void* userParameters) {
		                       const char* typeString = "UNKNOWN";
		                       switch (type) {
		                       case GL_DEBUG_TYPE_ERROR:
			                       typeString = "ERROR";
			                       break;
		                       case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			                       typeString = "DEPRECATED_BEHAVIOR";
			                       break;
		                       case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			                       typeString = "UNDEFINED_BEHAVIOR";
			                       break;
		                       case GL_DEBUG_TYPE_PORTABILITY:
			                       typeString = "PORTABILITY";
			                       break;
		                       case GL_DEBUG_TYPE_PERFORMANCE:
			                       typeString = "PERFORMANCE";
			                       break;
		                       case GL_DEBUG_TYPE_OTHER:
			                       typeString = "OTHER";
			                       break;
		                       }

		                       const char* severityString = "UNKNOWN";
		                       switch (severity) {
		                       case GL_DEBUG_SEVERITY_LOW:
			                       severityString = "LOW";
			                       break;
		                       case GL_DEBUG_SEVERITY_MEDIUM:
			                       severityString = "MEDIUM";
			                       break;
		                       case GL_DEBUG_SEVERITY_HIGH:
			                       severityString = "HIGH";
			                       break;
		                       }

		                       Log::error("GL CALLBACK: type = %s, severity = %s, message = %s\n", typeString, severityString, message);
	                       },
	                       0);

	// GLFW Settings
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Setup dark style
	ImGui::StyleColorsDark();
	setupImGuiStyle();
	setHue(255.0f / 255.0f);

	// Load notifications
	//ImGui::MergeIconsWithLatestFont(io.Fonts->ConfigData.back().SizePixels, true);

	settings.init();
	screen.init();

	return true;
}

void update() {
	// Get events
	glfwPollEvents();

	screen.update();
}

void render() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	bool fullscreen = true;
	if (fullscreen) {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	} else {
		dockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, windowFlags);
	ImGui::PopStyleVar();
	if (fullscreen)
		ImGui::PopStyleVar(2);

	ImGui::DockSpace(ImGui::GetID("DockSpaceID"), ImVec2(0.0f, 0.0f), dockspaceFlags);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit"))
				glfwSetWindowShouldClose(window, GLFW_TRUE);

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// Default windows
	ImGui::ShowDemoWindow();
	renderImGuiStyleEditor();

	screen.render();

	ImGui::End();

	// Render notifications
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));
	//ImGui::RenderNotifications();
	//ImGui::PopStyleVar(1);
	//ImGui::PopStyleColor(1);

	// Submit
	ImGui::Render();
	glfwGetFramebufferSize(window, &dimension.x, &dimension.y);
	glCall(glViewport(0, 0, dimension.x, dimension.y));
	glCall(glClearColor(1, 1, 1, 1));
	glCall(glClear(GL_COLOR_BUFFER_BIT));
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Swap
	glfwSwapBuffers(window);
}

void close() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	glfwDestroyWindow(window);
	glfwTerminate();
}

bool shouldClose() {
	return glfwWindowShouldClose(window) || ImGui::IsKeyDown(GLFW_KEY_ESCAPE);
}

int startApplication() {
	if (!init())
		return -1;

	// Main loop
	while (!shouldClose()) {
		update();
		render();
	}

	close();

	return 0;
}

void rotatedImageEqualityTest() {
	cv::Mat beethoven = cv::imread("../res/beethoven.png");
	cv::Mat alignedImage = Grayscale(beethoven).grayscale;
	//cv::Canny(alignedImage, alignedImage, 100, 150);
	cv::Mat rotatedImage = RotatedTexture::rotate(alignedImage, 45);
	cv::Rect patch(150, 250, 30, 30);
	cv::Mat rotatedTemplate = rotatedImage(patch);
	cv::Mat alignedTemplate = RotatedTexture::rotate(rotatedTemplate, -45);
	cv::Mat rotatedImageMask = RotatedTexture::rotate(cv::Mat(alignedImage.rows, alignedImage.cols, CV_8UC1, cv::Scalar(255)), 45);
	cv::Mat alignedTemplateMask = RotatedTexture::rotate(cv::Mat(rotatedTemplate.rows, rotatedTemplate.cols, CV_8UC1, cv::Scalar(255)), -45);
	cv::Mat itran = RotatedTexture::computeTransformationMatrix(cv::Size(patch.width, patch.height), -45);

	cv::Mat alignedResult;
	cv::matchTemplate(alignedImage, alignedTemplate, alignedResult, cv::TemplateMatchModes::TM_SQDIFF_NORMED, alignedTemplateMask.clone());

	cv::Mat rotatedResult;
	cv::matchTemplate(rotatedImage, rotatedTemplate, rotatedResult, cv::TemplateMatchModes::TM_SQDIFF_NORMED);

	double alignedMax;
	cv::Point alignedPoint;
	cv::minMaxLoc(alignedResult, &alignedMax, nullptr, &alignedPoint, nullptr);
	cv::rectangle(alignedImage, alignedPoint, alignedPoint + cv::Point(alignedTemplate.cols, alignedTemplate.rows), Colors::RGB_R.cv());
	cv::rectangle(alignedResult, alignedPoint, alignedPoint + cv::Point(alignedTemplate.cols, alignedTemplate.rows), Colors::RGB_R.cv());

	double rotatedMax;
	cv::Point rotatedPoint;
	cv::minMaxLoc(rotatedResult,
	              &rotatedMax,
	              nullptr,
	              &rotatedPoint,
	              nullptr,
	              rotatedImageMask(cv::Rect(0, 0, rotatedResult.cols, rotatedResult.rows)));
	cv::rectangle(rotatedImage, rotatedPoint, rotatedPoint + cv::Point(rotatedTemplate.cols, rotatedTemplate.rows), Colors::WHITE.cv());
	cv::rectangle(rotatedResult, rotatedPoint, rotatedPoint + cv::Point(rotatedTemplate.cols, rotatedTemplate.rows), Colors::WHITE.cv());

	std::vector<cv::Point2f> in = {rotatedPoint};
	cv::perspectiveTransform(in, in, itran);

	alignedPoint = cv::Point(in[0]);
	cv::rectangle(alignedImage, alignedPoint, alignedPoint + cv::Point(alignedTemplate.cols, alignedTemplate.rows), Colors::RGB_G.cv());
	cv::rectangle(alignedResult, alignedPoint, alignedPoint + cv::Point(alignedTemplate.cols, alignedTemplate.rows), Colors::RGB_G.cv());

	printf("%f, %f", alignedMax, rotatedMax);

	cv::imshow("Aligned Image", alignedImage);
	cv::imshow("Aligned Template", alignedTemplate);
	cv::imshow("Rotated Image", rotatedImage);
	cv::imshow("Rotated Template", rotatedTemplate);
	cv::imshow("Rotated Image Mask", rotatedImageMask);
	cv::imshow("Aligned Template Mask", alignedTemplateMask);
	cv::imshow("Aligned Result", alignedResult);
	cv::imshow("Rotated Result", rotatedResult);

	cv::waitKey();
}

void rotatedMaskEqualityTest() {
	cv::Mat beethoven = cv::imread("../res/beethoven.png");
	cv::Mat beethovenGray = Grayscale(beethoven).grayscale;
	cv::Mat alignedImage;
	cv::Canny(beethovenGray, alignedImage, 100, 150);
	cv::Mat alignedTemplate = alignedImage(cv::Rect(150, 250, 20, 30));
	cv::Mat rotatedTemplate = RotatedTexture::rotate(alignedTemplate, 45);
	cv::Mat rotatedImage = RotatedTexture::rotate(alignedImage, 45);
	cv::Mat rotatedImageMask = RotatedTexture::rotate(cv::Mat(alignedImage.rows, alignedImage.cols, CV_8UC1, cv::Scalar(255)), 15);
	cv::Mat rotatedTemplateMask = RotatedTexture::rotate(cv::Mat(alignedTemplate.rows, alignedTemplate.cols, CV_8UC1, cv::Scalar(255)), 15);

	cv::Mat alignedResult;
	cv::matchTemplate(alignedImage, alignedTemplate, alignedResult, cv::TemplateMatchModes::TM_SQDIFF_NORMED);

	cv::Mat rotatedResult;
	cv::matchTemplate(rotatedImage, rotatedTemplate, rotatedResult, cv::TemplateMatchModes::TM_SQDIFF_NORMED, rotatedTemplateMask.clone());

	double alignedMax;
	cv::Point alignedPoint;
	cv::minMaxLoc(alignedResult, &alignedMax, nullptr, &alignedPoint, nullptr);
	cv::rectangle(alignedImage, alignedPoint, alignedPoint + cv::Point(alignedTemplate.cols, alignedTemplate.rows), Colors::WHITE.cv());
	cv::rectangle(alignedResult, alignedPoint, alignedPoint + cv::Point(alignedTemplate.cols, alignedTemplate.rows), Colors::WHITE.cv());

	double rotatedMax;
	cv::Point rotatedPoint;
	cv::minMaxLoc(rotatedResult,
	              &rotatedMax,
	              nullptr,
	              &rotatedPoint,
	              nullptr,
	              rotatedImageMask(cv::Rect(0, 0, rotatedResult.cols, rotatedResult.rows)));
	cv::rectangle(rotatedImage, rotatedPoint, rotatedPoint + cv::Point(rotatedTemplate.cols, rotatedTemplate.rows), Colors::WHITE.cv());
	cv::rectangle(rotatedResult, rotatedPoint, rotatedPoint + cv::Point(rotatedTemplate.cols, rotatedTemplate.rows), Colors::WHITE.cv());
	printf("%f, %f", alignedMax, rotatedMax);

	cv::imshow("Aligned Image", alignedImage);
	cv::imshow("Aligned Template", alignedTemplate);
	cv::imshow("Rotated Image", rotatedImage);
	cv::imshow("Rotated Template", rotatedTemplate);
	cv::imshow("Rotated Image Mask", rotatedImageMask);
	cv::imshow("Rotated Template Mask", rotatedTemplateMask);
	cv::imshow("Aligned Result", alignedResult);
	cv::imshow("Rotated Result", rotatedResult);

	cv::waitKey();
}

void contours() {
	cv::RNG rng(12345);

	cv::Mat beethoven = cv::imread("../res/beethoven.png");
	cv::imshow("Beethoven", beethoven);
	cv::waitKey();

	cv::Mat grayscale;
	cv::cvtColor(beethoven, grayscale, cv::COLOR_BGR2GRAY);
	cv::imshow("Beethoven", grayscale);
	cv::waitKey();

	cv::Mat threshold;
	cv::threshold(grayscale, threshold, 125, 255, 0);
	cv::imshow("threshold", threshold);
	cv::waitKey();

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(threshold, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	cv::Mat drawing = cv::Mat::zeros(threshold.size(), CV_8UC3);
	for (size_t i = 0; i < contours.size(); i++) {
		cv::Scalar color = cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		drawContours(drawing, contours, static_cast<int>(i), color, 2, cv::LINE_8, hierarchy, 0);
	}
	cv::imshow("Contours", drawing);
	cv::waitKey();
}

void cornerHarris_demo(int, void*);
cv::Mat src;
int threshold = 200;

void rollingGuidanceLevels() {
	cv::Mat beethoven = cv::imread("../res/beethoven.png");
	cv::imshow("Original", beethoven);

	cv::Mat grayscale;
	cv::cvtColor(beethoven, grayscale, cv::COLOR_BGR2GRAY);

	cv::Mat grayscaleCanny;
	//grayscaleCanny = Sobel(grayscale, SobelType::MAGNITUDE, 1, 3).sobel;
	cv::Canny(grayscale, grayscaleCanny, 50, 150, 3);
	cv::imshow("grayscaleCanny", grayscaleCanny);

	cv::Mat canny(grayscale.rows, grayscale.cols, CV_32FC1, cv::Scalar(0));
	for (int i = 0; i < 10; i++) {
		double sigma = 11.0 - i * 1.0;
		cv::Mat guidance = RollingGuidanceFilter::filter(beethoven, sigma, 10, 4);
		cv::imshow("guidance", guidance);

		cv::Mat guidanceCanny;
		cv::Canny(guidance, guidanceCanny, 50, 150, 3);
		guidanceCanny.convertTo(guidanceCanny, CV_32FC1, 1.0 / (i + 1.0));

		//canny += guidanceCanny;
		cv::max(canny, guidanceCanny, canny);
	}

	cv::convertScaleAbs(canny, canny);
	cv::imshow("canny", canny);
	//cv::waitKey();

	cv::Mat dilated1;
	cv::Mat dilated2;
	double dilation_size_1 = 5;
	double dilation_size_2 = 6;
	cv::Mat element1 = cv::getStructuringElement(
		cv::MORPH_RECT,
		cv::Size(2 * dilation_size_1 + 1, 2 * dilation_size_1 + 1),
		cv::Point(dilation_size_1, dilation_size_1));
	cv::Mat element2 = cv::getStructuringElement(
		cv::MORPH_RECT,
		cv::Size(2 * dilation_size_2 + 1, 2 * dilation_size_2 + 1),
		cv::Point(dilation_size_2, dilation_size_2));
	cv::dilate(canny, dilated1, element1);
	cv::dilate(canny, dilated2, element2);
	cv::Mat dilated = dilated2 - dilated1;
	cv::imshow("dilated", dilated);

	cv::Mat image = dilated.clone();
	cv::Mat result = beethoven.clone();
	int n = 300;
	int thickness = 3;
	std::vector<double> values(n);
	for (int i = 0; i < n; i++) {
		int size = static_cast<int>(Utils::interpolate(31.0, 7.0, static_cast<double>(i) / n));
		cv::Mat horizontalLine(1, size, CV_8UC1, cv::Scalar(255));
		cv::Mat verticalLine(size, 1, CV_8UC1, cv::Scalar(255));

		cv::Mat matching;
		if (i % 2 == 0) {
			cv::matchTemplate(image, horizontalLine, matching, cv::TemplateMatchModes::TM_CCORR_NORMED);

			double value;
			cv::Point point;
			cv::minMaxLoc(matching, nullptr, &value, nullptr, &point);

			if (point.y == -1) {
				Log::debug("Abort");
				break;
			}

			values[i] = value;

			cv::line(result, point, cv::Point(point.x + size, point.y), cv::Scalar(0, 0, 255));
			cv::line(image, point, cv::Point(point.x + size, point.y), cv::Scalar(0, 0, 0), thickness);
		} else {
			cv::matchTemplate(image, verticalLine, matching, cv::TemplateMatchModes::TM_CCORR_NORMED);

			double value;
			cv::Point point;
			cv::minMaxLoc(matching, nullptr, &value, nullptr, &point);

			if (point.x == -1) {
				Log::debug("Abort");
				break;
			}

			values[i] = value;

			cv::line(result, point, cv::Point(point.x, point.y + size), cv::Scalar(0, 0, 255));
			cv::line(image, point, cv::Point(point.x, point.y + size), cv::Scalar(0, 0, 0), thickness);
		}

		//cv::imshow("Result", result);
		//cv::imshow("Image", image);
		//cv::waitKey();
	}

	cv::imshow("Result", result);

	int width = values.size();
	int height = width * 0.75;
	cv::Mat graph(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
	double max = *std::ranges::max_element(values);
	for (int x = 0; x < values.size(); x++) {
		int y = static_cast<int>(values[x] / max * height);
		cv::line(graph, cv::Point(x, height), cv::Point(x, height - y), cv::Scalar(0, 0, 255));
	}

	cv::imshow("Graph", graph);

	cv::waitKey();
}

void rg() {
	cv::Mat img = cv::imread("../res/eye_2.png");
	cv::Mat cannyLevels(img.rows, img.cols, CV_32FC1, cv::Scalar(0));
	for (int i = 0; i < 10; i++) {
		double sigma = 11.0 - i * 1.0;
		cv::Mat guidance = RollingGuidanceFilter::filter(img, sigma, 10, 4);

		cv::Mat guidanceCanny;
		cv::Canny(guidance, guidanceCanny, 50, 150, 3);

		/*if (i == 0 || i == 3 || i == 6 | i == 9) {
			cv::Mat c;
			cv::convertScaleAbs(guidanceCanny, c);

			cv::imwrite(std::string("../res/rg/guidance_") + std::to_string(i) + ".png", guidance);
			cv::imwrite(std::string("../res/rg/canny_") + std::to_string(i) + ".png", c*2);

		}*/

		guidanceCanny.convertTo(guidanceCanny, CV_32FC1, 1.0 / (i + 1.0));

		//canny += guidanceCanny;
		cv::max(cannyLevels, guidanceCanny, cannyLevels);
	}
	cv::convertScaleAbs(cannyLevels, cannyLevels);

	cv::Mat dilated1;
	cv::Mat dilated2;
	int dil = 5;
	cv::Mat element1 = cv::getStructuringElement(
		cv::MORPH_RECT,
		cv::Size(2 * (dil - 1) + 1, 2 * (dil - 1) + 1),
		cv::Point(dil, dil));
	cv::Mat element2 = cv::getStructuringElement(
		cv::MORPH_RECT,
		cv::Size(2 * dil + 1, 2 * dil + 1),
		cv::Point(dil, dil));
	cv::dilate(cannyLevels, dilated1, element1);
	cv::dilate(cannyLevels, dilated2, element2);
	cv::Mat dilated = dilated2 - dilated1;
	cv::imwrite("../res/rg/dilate1.png", dilated1);
	cv::imwrite("../res/rg/dilate2.png", dilated2);
	cv::imwrite("../res/rg/dilated.png", dilated);

	cv::imwrite("../res/rg/result.png", cannyLevels*2);
}

void eq() {
	cv::Mat source = cv::imread("../res/wood_3.png");
	cv::Mat target = cv::imread("../res/snow.jpg");

	// Target grayscale
	Grayscale targetGrayscale(target);
	Grayscale sourceGrayscale(source);

	// Calculate equalization
	Equalization equalization(targetGrayscale.grayscale, sourceGrayscale.grayscale);

	// Weighted equalization
	cv::Mat wequalized1;
	cv::Mat wequalized2;
	cv::Mat wequalized3;
	double w1 = 1.0;
	double w2 = 0.5;
	double w3 = 0.0;
	cv::addWeighted(targetGrayscale.grayscale, 1.0f - w1, equalization.equalization, w1, 0.0, wequalized1);
	cv::addWeighted(targetGrayscale.grayscale, 1.0f - w2, equalization.equalization, w2, 0.0, wequalized2);
	cv::addWeighted(targetGrayscale.grayscale, 1.0f - w3, equalization.equalization, w3, 0.0, wequalized3);
	
	cv::imwrite("../res/rg/eq_100.png", wequalized1);
	cv::imwrite("../res/rg/eq_50.png", wequalized2);
	cv::imwrite("../res/rg/eq_0.png", wequalized3);
}

void rollingGuidance() {
	cv::RNG rng(12345);

	cv::Mat beethoven = cv::imread("../res/beethoven.png");
	cv::imshow("Original", beethoven);

	cv::Mat grayscale;
	cv::cvtColor(beethoven, grayscale, cv::COLOR_BGR2GRAY);

	cv::Mat grayscaleCanny;
	//grayscaleCanny = Sobel(grayscale, SobelType::MAGNITUDE, 1, 3).sobel;
	cv::Canny(grayscale, grayscaleCanny, 50, 150, 3);
	cv::imshow("grayscaleCanny", grayscaleCanny);

	cv::Mat guidance = RollingGuidanceFilter::filter(beethoven, 5, 25.5, 4);
	cv::imshow("guidance", guidance);

	cv::Mat guidanceCanny;
	//guidanceCanny = Sobel(guidance, SobelType::MAGNITUDE, 1, 3).sobel;
	cv::Canny(guidance, guidanceCanny, 50, 150, 3);
	cv::imshow("guidanceCanny", guidanceCanny);

	cv::Mat guidanceTotal = guidance.clone();
	guidanceTotal.setTo(255, guidanceCanny);
	cv::imshow("GuidanceTotal", guidanceTotal);

	cv::Mat bilateral;
	cv::bilateralFilter(beethoven, bilateral, 15, 80, 80);
	cv::imshow("bilateral", bilateral);

	cv::Mat bilateralCanny;
	//bilateralCanny = Sobel(bilateral, SobelType::MAGNITUDE, 1, 3).sobel;
	cv::Canny(bilateral, bilateralCanny, 50, 150, 3);
	cv::imshow("bilateralCanny", bilateralCanny);

	cv::Mat b = bilateral.clone();
	b.setTo(255, bilateralCanny);
	cv::imshow("bilateralTotal", b);

	cv::Mat canny;
	cv::max(guidanceCanny, bilateralCanny / 2, canny);
	cv::imshow("canny", canny);

	cv::Mat image = canny.clone();
	cv::Mat result = guidance.clone();
	int n = 300;
	std::vector<double> values(n);
	for (int i = 0; i < n; i++) {
		int size = static_cast<int>(Utils::interpolate(31.0, 7.0, static_cast<double>(i) / n));
		cv::Mat horizontalLine(1, size, CV_8UC1, cv::Scalar(255));
		cv::Mat verticalLine(size, 1, CV_8UC1, cv::Scalar(255));

		cv::Mat matching;
		if (i % 2 == 0) {
			cv::matchTemplate(image, horizontalLine, matching, cv::TemplateMatchModes::TM_CCORR_NORMED);

			double value;
			cv::Point point;
			cv::minMaxLoc(matching, nullptr, &value, nullptr, &point);

			if (point.y == -1) {
				Log::debug("Abort");
				break;
			}

			values[i] = value;

			cv::line(result, point, cv::Point(point.x + size, point.y), cv::Scalar(0, 0, 255));
			cv::line(image, point, cv::Point(point.x + size, point.y), cv::Scalar(0, 0, 0), 3);
		} else {
			cv::matchTemplate(image, verticalLine, matching, cv::TemplateMatchModes::TM_CCORR_NORMED);

			double value;
			cv::Point point;
			cv::minMaxLoc(matching, nullptr, &value, nullptr, &point);

			if (point.x == -1) {
				Log::debug("Abort");
				break;
			}

			values[i] = value;

			cv::line(result, point, cv::Point(point.x, point.y + size), cv::Scalar(0, 0, 255));
			cv::line(image, point, cv::Point(point.x, point.y + size), cv::Scalar(0, 0, 0), 3);
		}
	}

	cv::imshow("Result", result);

	int width = values.size();
	int height = width * 0.75;
	cv::Mat graph(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
	double max = *std::ranges::max_element(values);
	for (int x = 0; x < values.size(); x++) {
		int y = static_cast<int>(values[x] / max * height);
		cv::line(graph, cv::Point(x, height), cv::Point(x, height - y), cv::Scalar(0, 0, 255));
	}

	cv::imshow("Graph", graph);

	/*
	 * Structuring elements
	 */
	//cv::Mat binary;
	//cv::adaptiveThreshold(canny, binary, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 15, -2);
	//cv::imshow("binary", binary);

	//cv::Mat horizontal = binary.clone();
	//cv::Mat vertical = binary.clone();

	//int horizontal_size = horizontal.cols / 30 /2;
	//// Create structure element for extracting horizontal lines through morphology operations
	//cv::Mat horizontalStructure = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(horizontal_size, 1));
	//cv::erode(horizontal, horizontal, horizontalStructure, cv::Point(-1, -1));
	//cv::dilate(horizontal, horizontal, horizontalStructure, cv::Point(-1, -1));
	//cv::imshow("horizontal", horizontal);

	//int vertical_size = vertical.rows / 30 / 2;
	//// Create structure element for extracting vertical lines through morphology operations
	//cv::Mat verticalStructure = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, vertical_size));
	//cv::erode(vertical, vertical, verticalStructure, cv::Point(-1, -1));
	//cv::dilate(vertical, vertical, verticalStructure, cv::Point(-1, -1));
	//cv::imshow("vertical", vertical);

	//cv::Mat both = horizontal + vertical;
	//cv::imshow("both", both);

	/*
	 * Houghlines
	 */
	//std::vector<cv::Vec4i> lines;
	//cv::HoughLinesP(canny, lines, 1, CV_PI / 180.0, 0, 0, 0);
	//Log::debug("%d", lines.size());

	//cv::Mat drawing;
	//cv::cvtColor(canny, drawing, cv::COLOR_GRAY2BGR);
	//for (size_t i = 0; i < lines.size(); i++) {
	//	cv::Vec4i l = lines[i];
	//	cv::line(drawing, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	//}
	//cv::imshow("Contours", drawing);

	/*guidance.convertTo(guidance, CV_32FC3);
	cv::Mat threshold;
	cv::threshold(guidance, threshold, 200, 255, cv::THRESH_BINARY_INV);
	cv::imshow("threshold", threshold);
	cv::waitKey();
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(threshold, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	cv::Mat output = guidance.clone();
	cv::drawContours(output, contours, -1, cv::Scalar(0, 0, 255), 1);

	cv::imshow("output", output);*/
	//cv::approxPolyDP()

	cv::waitKey();
}

void cornerHarris_demo(int, void*) {
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.04;
	cv::Mat res;
	src.copyTo(res);
	res.convertTo(res, CV_32FC3);
	cv::Mat dst = cv::Mat::zeros(src.size(), CV_32FC1);
	cornerHarris(src, dst, blockSize, apertureSize, k);
	cv::Mat dst_norm, dst_norm_scaled;
	normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
	for (int i = 0; i < dst_norm.rows; i++) {
		for (int j = 0; j < dst_norm.cols; j++) {
			if ((int)dst_norm.at<float>(i, j) > threshold) {
				circle(res, cv::Point(j, i), 4, cv::Scalar(255), 1, 8, 0);
			}
		}
	}
	cv::namedWindow("Corners");
	cv::imshow("Corners", res);
}

void houghLinesP() {
	cv::RNG rng(12345);

	cv::Mat beethoven = cv::imread("../res/beethoven.png");
	//cv::imshow("Beethoven", beethoven); cv::waitKey();

	cv::Mat grayscale;
	cv::cvtColor(beethoven, grayscale, cv::COLOR_BGR2GRAY);
	//cv::imshow("Beethoven", grayscale); cv::waitKey();

	cv::Mat threshold;
	cv::threshold(grayscale, threshold, 125, 255, 0);
	//cv::imshow("threshold", threshold); cv::waitKey();

	cv::Mat canny;
	cv::Canny(threshold, canny, 50, 150, 3);
	cv::imshow("Canny", canny);
	cv::waitKey();

	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(canny, lines, 1, CV_PI / 180.0, 15, 0, 0);
	Log::debug("%d", lines.size());

	cv::Mat drawing;
	cv::cvtColor(canny, drawing, cv::COLOR_GRAY2BGR);
	for (size_t i = 0; i < lines.size(); i++) {
		cv::Vec4i l = lines[i];
		cv::line(drawing, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	}

	cv::imshow("Contours", drawing);
	cv::waitKey();
}

void houghLines() {
	cv::RNG rng(12345);

	cv::Mat beethoven = cv::imread("../res/beethoven.png");
	//cv::imshow("Beethoven", beethoven); cv::waitKey();

	cv::Mat grayscale;
	cv::cvtColor(beethoven, grayscale, cv::COLOR_BGR2GRAY);
	//cv::imshow("Beethoven", grayscale); cv::waitKey();

	cv::Mat threshold;
	cv::threshold(grayscale, threshold, 125, 255, 0);
	//cv::imshow("threshold", threshold); cv::waitKey();

	cv::Mat canny;
	cv::Canny(threshold, canny, 50, 150, 3);
	cv::imshow("Canny", canny);
	cv::waitKey();

	std::vector<cv::Vec2f> lines;
	cv::HoughLines(canny, lines, 1, CV_PI / 180.0, 150, 0, 0);
	Log::debug("%d", lines.size());

	cv::Mat drawing;
	cv::cvtColor(canny, drawing, cv::COLOR_GRAY2BGR);
	for (size_t i = 0; i < lines.size(); i++) {
		float rho = lines[i][0], theta = lines[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		cv::line(drawing, pt1, pt2, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
	}

	cv::imshow("Contours", drawing);
	cv::waitKey();
}

void testWarp() {
	cv::Size size(500, 500);
	cv::Mat tr = Utils::computeTransformationMatrix(size, 45);
	cv::Mat itr;
	cv::invertAffineTransform(tr, itr);

	cv::Point ori(150, 250);
	cv::Point wrp = Utils::warp(ori, tr);
	cv::Point bck = Utils::warp(wrp, itr);
}

void testGabor() {
	GaborFilterBank bank;
	cv::Mat beethoven = cv::imread("../res/beethoven.png");
	std::vector<cv::Mat> gabors = bank.compute(beethoven);

	cv::Mat total = cv::Mat(gabors[0].rows, gabors[0].cols, gabors[0].type(), cv::Scalar(0));
	for (cv::Mat gabor : gabors) {
		cv::imshow("gabor", gabor);
		cv::waitKey();

		total += gabor;
	}

	cv::imshow("total", total);
	cv::waitKey();
}

void testColor() {
	bool color = false;

	cv::Mat target = cv::imread("../res/eye_2.png");
	cv::resize(target, target, cv::Size(target.cols / 2, target.rows / 2));
	cv::resize(target, target, cv::Size(target.cols / 2, target.rows / 2));
	cv::Mat targetGrayscale = color ? target : Grayscale(target).grayscale;
	cv::Mat source = cv::imread("../res/wood_3.png");
	cv::resize(source, source, cv::Size(source.cols / 2, source.rows / 2));
	cv::Mat sourceGrayscale = color ? source : Grayscale(source).grayscale;

	// Calculate equalization
	Equalization equalization(targetGrayscale, sourceGrayscale);
	cv::Mat equalized = equalization.equalization;

	// Weighted equalization
	cv::Mat wequalized;
	double weight = 0.5;
	cv::addWeighted(targetGrayscale, 1.0f - weight, equalized, weight, 0.0, wequalized);

	cv::Mat defaultPuzzle;
	cv::Mat equalizedPuzzle;
	cv::Mat weightedPuzzle;
	target.copyTo(defaultPuzzle);
	target.copyTo(equalizedPuzzle);
	target.copyTo(weightedPuzzle);

	cv::imshow("target", target);
	cv::imshow("source", source);

	int size = 10;
	for (int x = 0; x < target.cols; x += size) {
		for (int y = 0; y < target.rows; y += size) {
			cv::Rect targetRect(x, y, std::min(size, target.cols - x), std::min(size, target.rows - y));
			cv::Mat defaultPatch = targetGrayscale(targetRect);
			cv::Mat weightedPatch = wequalized(targetRect);
			cv::Mat equalizedPatch = equalized(targetRect);

			cv::Mat defaultResult;
			cv::Mat weightedResult;
			cv::Mat equalizedResult;
			cv::matchTemplate(sourceGrayscale, defaultPatch, defaultResult, cv::TemplateMatchModes::TM_SQDIFF_NORMED);
			cv::matchTemplate(sourceGrayscale, weightedPatch, weightedResult, cv::TemplateMatchModes::TM_SQDIFF_NORMED);
			cv::matchTemplate(sourceGrayscale, equalizedPatch, equalizedResult, cv::TemplateMatchModes::TM_SQDIFF_NORMED);

			double defaultScore;
			double weightedScore;
			double equalizedScore;
			cv::Point defaultPoint;
			cv::Point weightedPoint;
			cv::Point equalizedPoint;
			cv::minMaxLoc(defaultResult, &defaultScore, nullptr, &defaultPoint, nullptr);
			cv::minMaxLoc(weightedResult, &weightedScore, nullptr, &weightedPoint, nullptr);
			cv::minMaxLoc(equalizedResult, &equalizedScore, nullptr, &equalizedPoint, nullptr);

			cv::Rect sourceDefaultRect(defaultPoint.x, defaultPoint.y, targetRect.width, targetRect.height);
			cv::Rect sourceWeightedRect(weightedPoint.x, weightedPoint.y, targetRect.width, targetRect.height);
			cv::Rect sourceEqualizedRect(equalizedPoint.x, equalizedPoint.y, targetRect.width, targetRect.height);

			source(sourceDefaultRect).copyTo(defaultPuzzle(targetRect));
			source(sourceWeightedRect).copyTo(weightedPuzzle(targetRect));
			source(sourceEqualizedRect).copyTo(equalizedPuzzle(targetRect));

			cv::imshow("defaultPuzzle", defaultPuzzle);
			cv::imshow("weightedPuzzle", weightedPuzzle);
			cv::imshow("equalizedPuzzle", equalizedPuzzle);

			cv::waitKey(1);
		}
	}

	cv::waitKey();
}

void testInt() {
	bool color = false;
	cv::Mat target = cv::imread("../res/earth.jpg");
	cv::Mat source = cv::imread("../res/wood_3.png");
	cv::Mat targetGrayscale = color ? target : Grayscale(target).grayscale;
	cv::Mat sourceGrayscale = color ? source : Grayscale(source).grayscale;

	// Calculate equalization
	Equalization equalization(targetGrayscale, sourceGrayscale);
	cv::Mat equalized = equalization.equalization;

	// Weighted equalization
	cv::Mat wequalized;
	double weight = 1;
	cv::addWeighted(targetGrayscale, 1.0f - weight, equalized, weight, 0.0, wequalized);

	int x = 60;
	int y = 450;
	int size = 60;
	cv::Rect patch(cv::Point(x, y), cv::Point(x + size, y + size));
	cv::Mat troi = wequalized(patch);

	cv::Mat res;
	double minVal;
	cv::Point minLoc;
	cv::matchTemplate(sourceGrayscale, wequalized(patch), res, cv::TemplateMatchModes::TM_SQDIFF_NORMED);
	cv::minMaxLoc(res, &minVal, nullptr, &minLoc, nullptr);

	cv::Rect match(minLoc, cv::Point(minLoc.x + size, minLoc.y + size));
	cv::Mat sroi = source(match);

	cv::rectangle(target, patch, cv::Scalar(0, 0, 255), 3);
	cv::rectangle(source, match, cv::Scalar(0, 255, 0), 3);
	cv::imshow("target", target);
	cv::imshow("source", source);
	cv::imshow("troi", target(patch));
	cv::imwrite("../res/output/source.png", source);
	cv::imwrite("../res/output/target.png", target);
	cv::imwrite("../res/output/patch.png", target(patch));
	cv::imwrite("../res/output/match.png", source(match));
	//cv::imshow("res", res);
	cv::imshow("sroi", source(match));
	cv::waitKey();
}

void testSaliency() {
	auto saliency = cv::saliency::StaticSaliencyFineGrained::create();
	cv::Mat res;
	cv::Mat img = cv::imread("../res/bike.jpg");
	cv::resize(img, img, cv::Size(1000, 1000 / img.cols * img.rows));
	saliency->computeSaliency(img, res);
	cv::cvtColor(res, res, cv::COLOR_GRAY2RGB);

	cv::imwrite("../res/output/saliency.png", res);
	cv::imshow("res", res);
	cv::waitKey();
}

void testRollingGuidance() {
	cv::Mat img = cv::imread("../res/eye.jpg");
	// Canny levels
	cv::Mat cannyLevels(img.rows, img.cols, CV_32FC1, cv::Scalar(0));
	for (int i = 0; i < 10; i++) {
		double sigma = 11.0 - i * 1.0;
		cv::Mat guidance = RollingGuidanceFilter::filter(img, sigma, 25, 4);

		cv::Mat guidanceCanny;
		cv::Canny(guidance, guidanceCanny, 50, 150, 3);

		if (i == 0 || i == 4 || i == 8) {
			cv::imwrite("../res/output/rg_" + std::to_string(i) + ".png", guidance);
			cv::imwrite("../res/output/cn_" + std::to_string(i) + ".png", guidanceCanny);
			Log::debug("i=%d, s=%f, w=%f", i, sigma, 1.0 / (i + 1.0));
		}

		guidanceCanny.convertTo(guidanceCanny, CV_32FC1, 1.0 / (i + 1.0));
		cv::max(cannyLevels, guidanceCanny, cannyLevels);
	}
	cv::convertScaleAbs(cannyLevels, cannyLevels);
	cv::imwrite("../res/output/hierarchical.png", cannyLevels);

	cv::imshow("re", cannyLevels);
	cv::waitKey();
}

void testEdge() {
	cv::Mat img = cv::imread("../res/jonas.jpg");
	cv::Mat source = cv::imread("../res/wood_3.png");
	cv::Mat gray, sgray;
	cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	cv::cvtColor(source, sgray, cv::COLOR_BGR2GRAY);
	cv::Mat eq = Equalization(gray, sgray).equalization;

	cv::imshow("sgray", sgray);
	cv::imshow("gray", gray);
	cv::imshow("eq", eq);

	cv::Mat blurEq = Blur(eq).blur;
	cv::Mat edgeBlurEq = Sobel(blurEq, SobelType::MAGNITUDE).sobel;
	cv::imshow("SobelBlurEq", edgeBlurEq);

	std::vector bgr { cv::Mat(), cv::Mat(), cv::Mat() };
	cv::split(img, bgr);
	/*cv::imshow("b", bgr[0]);
	cv::imshow("g", bgr[1]);
	cv::imshow("r", bgr[2]);*/

	cv::Mat be = Sobel(bgr[0], SobelType::MAGNITUDE).sobel;
	cv::Mat ge = Sobel(bgr[1], SobelType::MAGNITUDE).sobel;
	cv::Mat re = Sobel(bgr[2], SobelType::MAGNITUDE).sobel;
	cv::Mat graye = Sobel(gray, SobelType::MAGNITUDE).sobel;
	cv::Mat result = re / 3 + ge / 3 + be / 3;

	/*cv::imshow("re", re);
	cv::imshow("ge", ge);
	cv::imshow("be", be);*/

	cv::imshow("graye", graye);
	cv::imshow("result", result);

	cv::waitKey();
}

void testMask() {
	cv::Mat mask(400, 600, CV_8UC1, cv::Scalar(255));
	cv::Rect rect(300, 200, 50, 50);
	mask(rect) = cv::Scalar(0);
	cv::Mat maskinv;
	cv::bitwise_not(mask, maskinv);
	cv::Mat res1;
	cv::Mat res2;
	cv::Mat res3;
	cv::boxFilter(maskinv,
	              res1,
	              -1,
	              cv::Size(rect.width, rect.height),
	              cv::Point(0, 0),
	              false,
	              cv::BORDER_ISOLATED);
	cv::threshold(res1, res2, 0, 255, cv::THRESH_BINARY);
	cv::bitwise_not(res2, res3);
	cv::imshow("mask", mask);
	cv::imshow("res1", res1);
	cv::imshow("res2", res2);
	cv::imshow("res3", res3);
	cv::waitKey();
}

int main(int, char**) {
	//startApplication();
	eq();
	//testMask();
	//testEdge();
	//rollingGuidanceLevels();
	//rollingGuidance();
	//testColor();
	//testInt();
	//testSaliency();
	//testRollingGuidance();

	return 0;

}
