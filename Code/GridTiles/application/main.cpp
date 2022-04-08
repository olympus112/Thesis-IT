#include "core.h"
#include "main.h"

#include <opencv2/highgui.hpp>

#include "graphics/imgui/imguiStyle.h"
#include "graphics/opencv/grayscale.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
//#include "imgui/imgui_notify.h"

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
	glfwSetErrorCallback([](int code, const char* error) { Log::error("%d: %s", code, error); });

	if (GLenum error = glewInit(); error != GLEW_OK) {
		Log::error("%s", glewGetErrorString(error));
		glfwTerminate();

		return false;
	}

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
	ImGui_ImplOpenGL3_Init("#version 130");

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
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else {
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
	glViewport(0, 0, dimension.x, dimension.y);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
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
	cv::minMaxLoc(rotatedResult, &rotatedMax, nullptr, &rotatedPoint, nullptr, rotatedImageMask(cv::Rect(0, 0, rotatedResult.cols, rotatedResult.rows)));
	cv::rectangle(rotatedImage, rotatedPoint, rotatedPoint + cv::Point(rotatedTemplate.cols, rotatedTemplate.rows), Colors::WHITE.cv());
	cv::rectangle(rotatedResult, rotatedPoint, rotatedPoint + cv::Point(rotatedTemplate.cols, rotatedTemplate.rows), Colors::WHITE.cv());

	std::vector<cv::Point2f> in = { rotatedPoint };
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
	cv::minMaxLoc(rotatedResult, &rotatedMax, nullptr, &rotatedPoint, nullptr, rotatedImageMask(cv::Rect(0, 0, rotatedResult.cols, rotatedResult.rows)));
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

int main(int, char**) {
	startApplication();

	//rotatedImageEqualityTest();
}
