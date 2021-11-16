#include "core.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Vec2i dimension;
GLFWwindow* window;

bool init() {
	// GLFW init
	if (!glfwInit()) {
		Log::error("GLFW init failed");

		return false;
	}

	// Window
	window = glfwCreateWindow(1200, 800, "Hello World", nullptr, nullptr);
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
	(void)io;

	// Setup dark style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	return true;
}

void update() {
	// Get events
	glfwPollEvents();
}

void render() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

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
