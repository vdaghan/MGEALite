// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "GUI/Initialisation.h"

#include "implot.h"

#include <chrono>
#include <thread>

void setupWindow() {
	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);
}

GUIInitialisation::GUIInitialisation(SharedSynchronisationToken && sST)
	: exitFlag(std::move(sST))
{};

void GUIInitialisation::initialise() {
	while (true) {
		// Setup window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
			continue;

		const char * glsl_version = "#version 410";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

		// Create window with graphics context
		bool fullScreen = false;
		if (fullScreen) {
			GLFWmonitor * monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode * mode = glfwGetVideoMode(monitor);
			window = glfwCreateWindow(mode->width, mode->height, "MGEALite", monitor, NULL);
		} else {
			GLFWmonitor * monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode * mode = glfwGetVideoMode(monitor);
			window = glfwCreateWindow(mode->width, mode->height, "MGEALite", NULL, NULL);
			glfwMaximizeWindow(window);
		}

		if (window == NULL)
			continue;

		glfwMakeContextCurrent(window);
		//glfwSwapInterval(1); // Enable vsync

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		imguiContext = ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO & io = ImGui::GetIO(); (void)io;
		io.IniFilename = NULL;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		ImFontConfig cfg;
		cfg.SizePixels = 16;
		io.Fonts->AddFontDefault(&cfg);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		break;
	}
}

void GUIInitialisation::GUILoop(std::function<void(void)> loop, bool & endLoop) {
	while (!exitFlag.get("exit")) {
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		if (endLoop) {
			break;
		}

		loop();

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

		ImGui::EndFrame();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}
