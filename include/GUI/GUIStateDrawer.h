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
enum class LogOrPlot {Log, Plot};
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

		int generationIndex;

		LogOrPlot logOrPlot;
		std::shared_ptr<GUILogger_mt> guiLoggerPtr;
		ImVec2 plotSize;
		ImVec2 plotGridShape;

		void drawTopRow(GUIState &);
		void drawProgressbarAndSlider(GUIState &);
		void drawLogsOrPlots(GUIState &);
		std::size_t getNumGenerations(GUIState &);
		std::size_t getLastGeneration(GUIState &);
		void drawFitnessVSIndividualsPlot(GUIState &);
		void drawFitnessVSGenerationsPlot(GUIState &);
		void drawVariationVSVariationStatistics(GUIState &);
		PlotMap plotMap;
};
