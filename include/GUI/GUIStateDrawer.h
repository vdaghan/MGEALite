#pragma once

#include "GUI/GUIFrameCounter.h"
#include "GUI/GUILogger.h"
#include "GUI/PlotData.h"
#include "SharedSynchronisation.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "GLFW/glfw3.h"

#include <list>
#include <string>

//using PlotFunction = std::function<void(GUIState&)>;
using PlotFunction = std::function<void(void)>;
using PlotMap = std::map<std::string, std::pair<bool, PlotFunction>>;
std::vector<std::string> getSelectedPlotFunctions(PlotMap const &);
int nearestLesserMultipleOfDivisorToNumber(int, int);
enum class LogOrPlot {Log, Plot};
class GUIStateDrawer {
	public:
		GUIStateDrawer(SharedSynchronisationToken && sST);
		void initialise(GLFWwindow *);
		
		void draw();
		PlotData plotData;
	private:
		SharedSynchronisationToken exitFlag;
		GLFWwindow * window;
		ImGuiStyle defaultPlotWindowStyle;
		ImGuiWindowFlags defaultPlotWindowFlags;
		ImPlotStyle defaultPlotStyle;
		ImPlotFlags defaultPlotFlags;
		ImPlotAxisFlags defaultPlotAxisFlags;
		ImPlotLineFlags defaultPlotLineFlags;

		MGEA::GUIFrameCounter guiFrameCounter;
		MGEA::FPSInfo fpsInfo;
		ImVec2 gridSize;
		ImVec2 halfGridSize;
		ImVec2 windowTopLeft;
		ImVec2 toGridPosition(float x, float y);

		std::size_t generationIndex;

		LogOrPlot logOrPlot;
		std::shared_ptr<GUILogger_mt> guiLoggerPtr;
		ImVec2 plotSize;
		ImVec2 plotGridShape;

		void drawTopRow();
		void drawProgressbarAndSlider();
		void drawLogsOrPlots();
		void drawFitnessVSIndividualsPlot();
		void drawFitnessVSGenerationsPlot();
		void drawVariationVSVariationStatistics();
		void drawVariationStatisticsOverGenealogy();
		void drawDistancesVSGenerations();
		void drawDistancesVSIndividuals();
		PlotMap plotMap;
};
