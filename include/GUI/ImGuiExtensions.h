#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include <string>

namespace ImGui {
	IMGUI_API bool InputTextMultiline(std::string label, std::string * str, const ImVec2 & size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void * user_data = NULL);
}