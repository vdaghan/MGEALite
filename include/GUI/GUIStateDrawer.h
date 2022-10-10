#pragma once

#include "GUI/GUIState.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "GLFW/glfw3.h"

using PlotFunction = std::function<void(GUIState &)>;
using PlotMap = std::map<std::string, std::pair<bool, PlotFunction>>;
std::vector<std::string> getSelectedPlotFunctions(PlotMap const &);
int nearestLesserMultipleOfDivisorToNumber(int, int);
class GUIStateDrawer {
	public:
		GUIStateDrawer();
		void initialise(GLFWwindow *);
		void draw(GUIState &);
	private:
		GLFWwindow * window;
		ImGuiStyle defaultPlotWindowStyle;
		ImGuiWindowFlags defaultPlotWindowFlags;
		ImPlotStyle defaultPlotStyle;
		ImPlotFlags defaultPlotFlags;
		ImPlotAxisFlags defaultPlotAxisFlags;
		ImPlotLineFlags defaultPlotLineFlags;
		ImVec2 halfGridSize;
		ImVec2 gridSize;
		bool hideLog;
		bool hidePlotSelection;
		ImVec2 plotSize;
		int generationIndex;

		std::size_t getNumGenerations(GUIState &);
		std::size_t getLastGeneration(GUIState &);
		void drawFitnessVSIndividualsPlot(GUIState &);
		void drawFitnessVSGenerationsPlot(GUIState &);
		PlotMap plotMap;
};
