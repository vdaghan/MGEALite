#pragma once

#include "GUI/GUIState.h"

#include "imgui.h"
#include "implot.h"

class GUIStateDrawer {
	public:
		void initialise();
		void draw(GUIState &);
	private:
		ImGuiStyle defaultPlotWindowStyle;
		ImGuiWindowFlags defaultPlotWindowFlags;
		ImPlotStyle defaultPlotStyle;
		ImPlotFlags defaultPlotFlags;
		ImPlotAxisFlags defaultPlotAxisFlags;
		ImPlotLineFlags defaultPlotLineFlags;
		void drawPlots(GUIState &);
};
