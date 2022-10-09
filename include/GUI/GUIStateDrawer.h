#pragma once

#include "GUI/GUIState.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"

using PlotFunction = std::function<void(GUIState &)>;
using PlotMap = std::map<std::string, std::pair<bool, PlotFunction>>;
std::vector<std::string> getSelectedPlotFunctions(PlotMap const &);
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

		std::size_t getNumGenerations(GUIState &);
		std::size_t getLastGeneration(GUIState &);
		void drawFitnessVSIndividualsPlot(GUIState &);
		void drawFitnessVSGenerationsPlot(GUIState &);
		PlotMap plotMap;
};
