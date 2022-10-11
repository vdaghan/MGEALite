#pragma once

#include "GUI/GUILogger.h"
#include "GUI/GUIState.h"
#include "SharedSynchronisation.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "GLFW/glfw3.h"

#include <list>
#include <string>

using PlotFunction = std::function<void(GUIState &)>;
using PlotMap = std::map<std::string, std::pair<bool, PlotFunction>>;
std::vector<std::string> getSelectedPlotFunctions(PlotMap const &);
int nearestLesserMultipleOfDivisorToNumber(int, int);
class GUIStateDrawer {
	public:
		GUIStateDrawer(SharedSynchronisationToken && sST);
		void initialise(GLFWwindow *);
		void draw(GUIState &);
	private:
		SharedSynchronisationToken exitFlag;
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
		std::shared_ptr<GUILogger_mt> guiLoggerPtr;
		bool hidePlotSelection;
		ImVec2 plotSize;
		int generationIndex;

		void drawProgressbarAndSlider(GUIState &);
		std::size_t getNumGenerations(GUIState &);
		std::size_t getLastGeneration(GUIState &);
		void drawFitnessVSIndividualsPlot(GUIState &);
		void drawFitnessVSGenerationsPlot(GUIState &);
		PlotMap plotMap;
};
