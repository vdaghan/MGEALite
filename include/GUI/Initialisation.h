#pragma once

#include <functional>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"

static void glfw_error_callback(int error, const char * description) {
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void setupWindow();

class GUIInitialisation {
	public:
		void GUILoop(std::function<void(void)> loop, bool & endLoop);
	private:
		GLFWwindow * window;
		ImGuiContext * imguiContext;
};
