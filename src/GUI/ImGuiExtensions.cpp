// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "GUI/ImGuiExtensions.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui {
	struct InputTextCallback_UserData {
		std::string * Str;
		ImGuiInputTextCallback ChainCallback;
		void * ChainCallbackUserData;
	};

	static int InputTextCallback(ImGuiInputTextCallbackData * data) {
		InputTextCallback_UserData * user_data = (InputTextCallback_UserData *)data->UserData;
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			// Resize string callback
			// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
			std::string * str = user_data->Str;
			IM_ASSERT(data->Buf == str->c_str());
			str->resize(data->BufTextLen);
			data->Buf = (char *)str->c_str();
		} else if (user_data->ChainCallback) {
			// Forward to user callback, if any
			data->UserData = user_data->ChainCallbackUserData;
			return user_data->ChainCallback(data);
		}
		return 0;
	}

	bool InputTextMultiline(std::string label, std::string * str, const ImVec2 & size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void * user_data) {
		IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		flags |= ImGuiInputTextFlags_CallbackResize;

		InputTextCallback_UserData cb_user_data = {
			.Str = str,
			.ChainCallback = callback,
			.ChainCallbackUserData = user_data
		};
		return InputTextMultiline(label.c_str(), (char *)str->c_str(), str->capacity() + 1, size, flags, InputTextCallback, &cb_user_data);
	}
}
