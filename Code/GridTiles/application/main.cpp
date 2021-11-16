#include "core.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "view/screen.h"

Vec2i dimension;

GLFWwindow* window;
URef<Screen> screen;


bool init() {
	// GLFW init
	if (!glfwInit()) {
		Log::error("GLFW init failed");

		return false;
	}

	// Window
	window = glfwCreateWindow(1200, 800, "Grid Tiles", nullptr, nullptr);
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

	// Setup dark style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	screen = std::make_unique<Screen>();
	screen->init();

	return true;
}

void update() {
	// Get events
	glfwPollEvents();

	screen->update();
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
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
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

	screen->render();
	
	ImGui::End();

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

int main(int, char**) {
	if (!init())
		return -1;

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		update();
		render();
	}

	close();

	return 0;
}