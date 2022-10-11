#pragma once

#include "SharedSynchronisation.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"

#include <functional>

static void glfw_error_callback(int error, const char * description) {
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void setupWindow();

// All methods of this struct should be called from the same thread
struct GUIInitialisation {
	GUIInitialisation(SharedSynchronisationToken && sST);
	void initialise();
	void GUILoop(std::function<void(void)> loop, bool & endLoop);
	GLFWwindow * window;
	ImGuiContext * imguiContext;
	private:
		SharedSynchronisationToken exitFlag;
};
