#include "GUI/GUIStateDrawer.h"

#include "GUI/Initialisation.h"
#include "GUI/ImGuiExtensions.h"
#include "GUI/ImPlotExtensions.h"

#include <algorithm>
#include <limits>

std::vector<std::string> getSelectedPlotFunctions(PlotMap const & plotMap) {
	std::vector<std::string> selected;
	for (auto & plotNamePair : plotMap) {
		auto & plotName = plotNamePair.first;
		auto & plotBoolFunction = plotNamePair.second;
		auto & plotBool = plotBoolFunction.first;
		if (plotBool) {
			selected.push_back(plotName);
		}
	}
	return selected;
}

int nearestLesserMultipleOfDivisorToNumber(int divisor, int number) {
	int mod = number % divisor;
	if (0 == mod) {
		return number;
	}
	return number - mod;
}

GUIStateDrawer::GUIStateDrawer(SharedSynchronisationToken && sST)
	: exitFlag(std::move(sST))
	, generationIndex(0)
	, logOrPlot(LogOrPlot::Log)
{
	guiLoggerPtr = std::make_shared<GUILogger_mt>();
	guiLoggerPtr->setLogLength(100);
	guiLoggerPtr->addToDefaultLogger(guiLoggerPtr);
}

void GUIStateDrawer::initialise(GLFWwindow * w) {
	window = w;
	defaultPlotWindowStyle = ImGui::GetStyle();
	defaultPlotWindowStyle.WindowPadding = ImVec2(0, 0);
	defaultPlotWindowStyle.WindowBorderSize = 0.0;
	defaultPlotWindowStyle.ChildBorderSize = 0.0;
	defaultPlotWindowStyle.FramePadding = ImVec2(0, 0);
	defaultPlotWindowStyle.FrameBorderSize = 0.0;
	//defaultPlotWindowStyle.FrameRounding = 100.;
	defaultPlotWindowStyle.CellPadding = ImVec2(0, 0);
	defaultPlotWindowStyle.ItemSpacing = ImVec2(0, 0);
	defaultPlotWindowStyle.ItemInnerSpacing = ImVec2(0, 0);
	defaultPlotWindowStyle.ButtonTextAlign = ImVec2(0.5f, 0.5f);

	defaultPlotWindowFlags = ImGuiWindowFlags_NoTitleBar
						   | ImGuiWindowFlags_NoResize
						   | ImGuiWindowFlags_NoMove
						   | ImGuiWindowFlags_NoCollapse
						   | ImGuiWindowFlags_NoSavedSettings;

	defaultPlotFlags = ImPlotFlags_NoTitle
					 | ImPlotFlags_NoMenus
					 | ImPlotFlags_Crosshairs;
	defaultPlotAxisFlags = ImPlotAxisFlags_NoGridLines
						 | ImPlotAxisFlags_NoMenus
						 | ImPlotAxisFlags_NoSideSwitch
						 | ImPlotAxisFlags_NoHighlight;
	defaultPlotLineFlags = ImPlotLineFlags_NoClip;

	guiFrameCounter.setFPSLimit(30.0);
	guiFrameCounter.setMeasurementInterval(5000);

	plotMap.emplace(std::make_pair("Fitness vs Individuals", std::make_pair(true, std::bind_front(&GUIStateDrawer::drawFitnessVSIndividualsPlot, this))));
	plotMap.emplace(std::make_pair("Fitness vs Generations", std::make_pair(true, std::bind_front(&GUIStateDrawer::drawFitnessVSGenerationsPlot, this))));
	plotMap.emplace(std::make_pair("VariationSuccess vs Variations", std::make_pair(true, std::bind_front(&GUIStateDrawer::drawVariationVSVariationStatistics, this))));
	plotMap.emplace(std::make_pair("Distances vs Generations", std::make_pair(true, [&]() {drawDistancesVSGenerations(); })));
	plotMap.emplace(std::make_pair("Distances vs Individuals", std::make_pair(true, std::bind_front(&GUIStateDrawer::drawDistancesVSIndividuals, this))));
	plotMap.emplace(std::make_pair("VariationStatisticsOverGenealogy", std::make_pair(true, std::bind_front(&GUIStateDrawer::drawVariationStatisticsOverGenealogy, this))));
}

void GUIStateDrawer::draw() {
	if (glfwWindowShouldClose(window)) {
		exitFlag.setAndCall("stop", FlagSetType::True);
		exitFlag.setAndCall("exit", FlagSetType::True);
	}
	fpsInfo = guiFrameCounter.tickAndWait();

	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	double windowWidthDouble = static_cast<double>(windowWidth);
	double windowHeightDouble = static_cast<double>(windowHeight);
	double aspectRatio = windowWidthDouble / windowHeightDouble;
	if (aspectRatio > 16.0 / 9.0) {
		windowHeight = nearestLesserMultipleOfDivisorToNumber(9, windowHeight);
		windowWidth = 16 * windowHeight / 9;
	} else if (aspectRatio < 16.0 / 9.0) {
		windowWidth = nearestLesserMultipleOfDivisorToNumber(16, windowWidth);
		windowHeight = 9 * windowWidth / 16;
	}
	ImVec2 offset(windowWidthDouble - static_cast<double>(windowWidth), windowHeightDouble - static_cast<double>(windowHeight));
	offset.x /= 2;
	offset.y /= 2;
	gridSize = ImVec2(static_cast<double>(windowWidth) / 16.0, static_cast<double>(windowHeight) / 9.0);
	halfGridSize = ImVec2(gridSize.x / 2, gridSize.y / 2);

	plotSize = ImVec2(4 * gridSize.x, 3 * gridSize.y);
	defaultPlotStyle = ImPlot::GetStyle();
	defaultPlotStyle.PlotPadding = ImVec2(10, 10);
	defaultPlotStyle.LabelPadding = ImVec2(10, 10);
	defaultPlotStyle.PlotDefaultSize = plotSize;
	defaultPlotStyle.PlotMinSize = plotSize;
	
	ImGui::GetStyle() = defaultPlotWindowStyle;
	ImPlot::GetStyle() = defaultPlotStyle;

	ImGui::SetNextWindowPos(ImVec2(offset.x, offset.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(16 * gridSize.x, 9 * gridSize.y), ImGuiCond_Always);
	ImGui::Begin("##Main", NULL, defaultPlotWindowFlags);
	windowTopLeft = ImGui::GetCursorPos();
	drawTopRow();
	ImGui::NewLine();
	drawProgressbarAndSlider();
	ImGui::NewLine();
	drawLogsOrPlots();
	ImGui::End();
}

ImVec2 GUIStateDrawer::toGridPosition(float x, float y) {
	return ImVec2(x * gridSize.x + windowTopLeft.x, y * gridSize.y + windowTopLeft.y);
};

void GUIStateDrawer::drawTopRow() {
	ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 3 * gridSize . x);
	std::string limitedFramerateText(std::format("Limited Framerate: {:5.2f}fps", fpsInfo.limitedFPS));
	ImGui::Text(limitedFramerateText.c_str());
	ImGui::NewLine();
	ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 3 * gridSize.x);
	std::string unlimitedFramerateText(std::format("Unlimited Framerate: {:5.2f}fps", fpsInfo.unlimitedFPS));
	ImGui::Text(unlimitedFramerateText.c_str());

	ImGui::SetCursorPos(toGridPosition(12.0, 0.0));
	bool startButtonClicked = ImGui::Button("Start", ImVec2(gridSize.x, gridSize.y));
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to start simulation");
	}
	if (startButtonClicked) {
		exitFlag.setAndCall("start", FlagSetType::True);
	}
	ImGui::SameLine();

	bool pauseButtonClicked = ImGui::Button("Pause", ImVec2(gridSize.x, gridSize.y));
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to pause simulation");
	}
	if (pauseButtonClicked) {
		exitFlag.setAndCall("pause", FlagSetType::Toggle);
	}
	ImGui::SameLine();

	bool stopButtonClicked = ImGui::Button("Stop", ImVec2(gridSize.x, gridSize.y));
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to stop simulation");
	}
	if (stopButtonClicked) {
		exitFlag.setAndCall("stop", FlagSetType::True);
	}
	ImGui::SameLine();

	bool exitButtonClicked = ImGui::Button("Exit", ImVec2(gridSize.x, gridSize.y));
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to stop simulation and quit application");
	}
	if (exitButtonClicked) {
		exitFlag.setAndCall("stop", FlagSetType::True);
		exitFlag.setAndCall("exit", FlagSetType::True);
	}
}

void GUIStateDrawer::drawProgressbarAndSlider() {
	std::optional<DEvA::EAStatistics<Spec>> eaStatistics;// = state.getEAStatistics();

	bool progressbarDataReady = eaStatistics.has_value();
	bool sliderDataReady = eaStatistics.has_value();
	DEvA::EAProgress eaProgress = progressbarDataReady ? eaStatistics->eaProgress : DEvA::EAProgress{};

	static bool lastGenerationSelected(true);
	ImVec4 activeButtonColor(GImGui->Style.Colors[ImGuiCol_ButtonActive]);
	ImVec4 buttonColor(GImGui->Style.Colors[ImGuiCol_Button]);
	if (lastGenerationSelected) {
		ImGui::PushStyleColor(ImGuiCol_Button, activeButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
	} else {
		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeButtonColor);
	}
	ImGui::SetCursorPos(toGridPosition(0.0, 1.0));
	bool trackLastGenerationButtonClicked = ImGui::Button("Track\nLast\nGeneration", ImVec2(gridSize.x, gridSize.y));
	ImGui::PopStyleColor(2);
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to select last generation for generation-dependent plots");
	}
	if (trackLastGenerationButtonClicked) {
		lastGenerationSelected = not lastGenerationSelected;
	}

	ImGui::SetCursorPos(toGridPosition(1.5, 1.0));
	auto fraction(plotData.progressbarData.fraction());
	auto progressText(plotData.progressbarData.text());
	ImGui::ProgressBar(fraction, ImVec2(14.5 * gridSize.x, halfGridSize.y), progressText.c_str());

	ImGui::SetCursorPos(toGridPosition(1.5, 1.5));
	auto& generationIndex(plotData.sliderData.generationIndex(lastGenerationSelected));
	auto currentGeneration(plotData.sliderData.currentGeneration());
	ImGui::PushItemWidth(14.5 * gridSize.x);
	bool sliderValueChanged = ImGui::SliderInt("##Generation", &generationIndex, 0, currentGeneration);
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Select target generation for generation-dependent plots");
	}
}

void GUIStateDrawer::drawLogsOrPlots() {
	ImGui::SetCursorPos(toGridPosition(0.0, 2.0));

	if (LogOrPlot::Log == logOrPlot) {
		static float wrapWidth = 1.0;
		wrapWidth = static_cast<float>(5.5 * gridSize.x);
		std::string logText;
		std::size_t totalLength(0);
		auto logs = guiLoggerPtr->getLogs();
		for (auto it(logs.rbegin()); it != logs.rend(); ++it) {
			logText += *it;
		}
		ImVec2 textSize(15 * gridSize.x, 7 * gridSize.y);
		ImGuiWindowFlags logWindowFlags = defaultPlotWindowFlags
			| ImGuiWindowFlags_HorizontalScrollbar;
		ImGuiInputTextFlags loggerFlags =
			ImGuiInputTextFlags_ReadOnly
			| ImGuiInputTextFlags_NoUndoRedo;
		ImGui::InputTextMultiline("##Logger", &logText, textSize, loggerFlags);
		ImGui::SetScrollHereY(1.0f);
		ImGui::SetCursorPos(toGridPosition(15.0, 2.0));
	}

	std::string hideLogText = (LogOrPlot::Log == logOrPlot) ? "<##HideLog" : ">##HideLog";
	bool hideLogClicked = ImGui::Button(hideLogText.c_str(), ImVec2(gridSize.x, 7 * gridSize.y));
	if (ImGui::IsItemHovered()) {
		if (LogOrPlot::Log == logOrPlot) {
			ImGui::SetTooltip("Press to show plots");
		} else {
			ImGui::SetTooltip("Press to show logs");
		}
	}
	if (hideLogClicked) {
		if (LogOrPlot::Log == logOrPlot) {
			logOrPlot = LogOrPlot::Plot;
		} else {
			logOrPlot = LogOrPlot::Log;
		}
	}

	if (LogOrPlot::Plot == logOrPlot) {
		ImGuiWindowFlags plotSelectionWindowFlags = defaultPlotWindowFlags;
		ImGui::SetCursorPos(toGridPosition(1.0, 2.0));
		ImGui::BeginChild("##PlotSelectionWindow", ImVec2(3 * gridSize.x, 7 * gridSize.y), false, plotSelectionWindowFlags);
		ImGui::BeginGroup();
		for (auto & plotNamePair : plotMap) {
			auto & plotName = plotNamePair.first;
			auto & plotBoolFunction = plotNamePair.second;
			auto & plotBool = plotBoolFunction.first;
			ImGui::Selectable(plotName.c_str(), &plotBool);
		}
		ImGui::EndGroup();
		ImGui::EndChild();

		ImGuiWindowFlags plotWindowFlags = defaultPlotWindowFlags;
		ImGui::SetCursorPos(toGridPosition(4.0, 2.0));
		ImGui::BeginChild("##PlotWindow", ImVec2(12 * gridSize.x, 7 * gridSize.y), false, plotWindowFlags);
		ImVec2 origin = ImGui::GetCursorPos();
		int col(0);
		int row(0);
		auto selected = getSelectedPlotFunctions(plotMap);
		for (auto & plotName : selected) {
			auto & plotPair = plotMap.at(plotName);
			auto & plotFunction = plotPair.second;
			ImVec2 cursorPos(origin.x + col * plotSize.x, origin.y + row * plotSize.y);
			ImGui::SetCursorPos(cursorPos);
			plotFunction();
			++col;
			if (col > 2) {
				col = 0;
				++row;
			}
			if (row > 2) {
				break;
			}
		}
		ImGui::EndChild();
	}
}

ImPlotPoint fitnessGetter(int idx, void * user_data) {
	std::vector<double> * userDataAsVector = static_cast<std::vector<double> *>(user_data);
	return ImPlotPoint(idx, userDataAsVector->at(idx));
};

void GUIStateDrawer::drawFitnessVSIndividualsPlot() {
	if (ImPlot::BeginPlot("FitnessVSIndividuals", plotSize, defaultPlotFlags)) {
		ImPlot::SetupAxis(ImAxis_X1, "Individual", defaultPlotAxisFlags);
		ImPlot::SetupAxis(ImAxis_Y1, "Fitness", defaultPlotAxisFlags);
		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);

		MGEA::FitnessData & fitnessData = plotData.fitnessData;
		if (0 == fitnessData.numberOfGenerations) {
			ImPlot::EndPlot();
			return;
		}

		double lastGenerationNumberOfIndividualsDouble(static_cast<double>(fitnessData.lastGenerationNumberOfIndividuals));
		double & lastGenerationMinimumFitness(fitnessData.lastGenerationMinimumFitness);
		double & lastGenerationMaximumFitness(fitnessData.lastGenerationMaximumFitness);
		double & lastGenerationDiff(fitnessData.lastGenerationDiff);
		auto & lastGenerationFitnesses(fitnessData.lastGenerationFitnesses);
		int lastGenerationNumberOfIndividualsInt(static_cast<int>(fitnessData.lastGenerationNumberOfIndividuals));

		ImPlot::SetupAxisLimits(ImAxis_X1, 0, lastGenerationNumberOfIndividualsDouble, ImPlotCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, lastGenerationMinimumFitness - 0.1 * lastGenerationDiff, lastGenerationMaximumFitness + 0.1 * lastGenerationDiff, ImPlotCond_Always);
		ImPlot::SetupFinish();

		ImPlot::PlotLine("Last Generation Fitness", &lastGenerationFitnesses[0], lastGenerationNumberOfIndividualsInt, 1.0, 0.0, defaultPlotLineFlags);
		ImPlot::EndPlot();
	}
}

void GUIStateDrawer::drawFitnessVSGenerationsPlot() {
	if (ImPlot::BeginPlot("Fitness Evolution", plotSize, defaultPlotFlags)) {
		ImPlot::SetupAxis(ImAxis_X1, "Generation", defaultPlotAxisFlags);
		ImPlot::SetupAxis(ImAxis_Y1, "Fitness", defaultPlotAxisFlags);
		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);

		MGEA::FitnessData & fitnessData = plotData.fitnessData;
		if (0 == fitnessData.numberOfGenerations) {
			ImPlot::EndPlot();
			return;
		}
		auto numberOfGenerationsDouble(static_cast<double>(fitnessData.numberOfGenerations));
		auto & lastGenerationMinimumFitness(fitnessData.lastGenerationMinimumFitness);
		auto & lastGenerationMaximumFitness(fitnessData.lastGenerationMaximumFitness);
		auto & lastGenerationDiff(fitnessData.lastGenerationDiff);
		auto & minimumOfGenerations(fitnessData.minimumOfGenerations);
		auto & maximumOfGenerations(fitnessData.maximumOfGenerations);
		auto & meanOfGenerations(fitnessData.meanOfGenerations);
		auto numberOfGenerationsInt(static_cast<int>(fitnessData.numberOfGenerations));

		ImPlot::SetupAxisLimits(ImAxis_X1, 0, numberOfGenerationsDouble, ImPlotCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, lastGenerationMinimumFitness - 0.5 * lastGenerationDiff, lastGenerationMaximumFitness + 0.5 * lastGenerationDiff, ImPlotCond_Always);
		ImPlot::SetupFinish();

		ImPlot::PlotShadedG("All generations fitness interval", fitnessGetter, &minimumOfGenerations, fitnessGetter, &maximumOfGenerations, numberOfGenerationsInt);
		ImPlot::PlotLine("All generations mean fitness", &meanOfGenerations[0], numberOfGenerationsInt, 1.0, 0.0, defaultPlotLineFlags);

		ImPlot::EndPlot();
	}
}

void GUIStateDrawer::drawVariationVSVariationStatistics() {
	if (ImPlot::BeginPlot("Variation Success", plotSize, defaultPlotFlags)) {
		ImPlotAxisFlags axisFlags =
			ImPlotAxisFlags_NoGridLines
			| ImPlotAxisFlags_NoMenus
			| ImPlotAxisFlags_NoSideSwitch
			| ImPlotAxisFlags_NoHighlight
			| ImPlotAxisFlags_AutoFit;
		ImPlot::SetupAxis(ImAxis_X1, "Variation Success", axisFlags);
		ImPlot::SetupAxis(ImAxis_Y1, "Variation", axisFlags);
		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);

		MGEA::VariationData & variationData = plotData.variationData;

		if (!variationData.lastGenerationVariationNames.empty()) {
			auto & tickValues(variationData.lastGenerationTicks);
			auto & variationNames(variationData.lastGenerationVariationNames);
			auto & successRates(variationData.lastGenerationSuccessRate);

			ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 1.0, ImPlotCond_Always);
			ImPlot::SetupAxisTicks(ImAxis_Y1, tickValues, variationNames, false);
			ImPlot::SetupAxisLimits(ImAxis_Y1, -1.0, double(tickValues.size()), ImPlotCond_Always);
			ImPlot::SetupFinish();

			ImPlot::PlotBars("Variation Success over Generation", &successRates[0], successRates.size(), 1.0, 0.0, ImPlotBarsFlags_Horizontal);
		}

		ImPlot::EndPlot();
	}
}

void GUIStateDrawer::drawVariationStatisticsOverGenealogy() {
	if (ImPlot::BeginPlot("Variation Success over Genealogy", plotSize, defaultPlotFlags)) {
		ImPlotAxisFlags axisFlags =
			ImPlotAxisFlags_NoGridLines
			| ImPlotAxisFlags_NoMenus
			| ImPlotAxisFlags_NoSideSwitch
			| ImPlotAxisFlags_NoHighlight
			| ImPlotAxisFlags_AutoFit;
		ImPlot::SetupAxis(ImAxis_X1, "Variation Success", axisFlags);
		ImPlot::SetupAxis(ImAxis_Y1, "Variation", axisFlags);
		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);

		MGEA::VariationData & variationData = plotData.variationData;

		if (!variationData.allGenerationsVariationNames.empty()) {
			auto & tickValues(variationData.allGenerationsTicks);
			auto & variationNames(variationData.allGenerationsVariationNames);
			auto & successRates(variationData.allGenerationsSuccessRate);

			ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 1.0, ImPlotCond_Always);
			ImPlot::SetupAxisTicks(ImAxis_Y1, tickValues, variationNames, false);
			ImPlot::SetupAxisLimits(ImAxis_Y1, -1.0, double(tickValues.size()), ImPlotCond_Always);
			ImPlot::SetupFinish();

			ImPlot::PlotBars("Variation Success over Genealogy", &successRates[0], successRates.size(), 1.0, 0.0, ImPlotBarsFlags_Horizontal);
		}

		ImPlot::EndPlot();
	}
}

void GUIStateDrawer::drawDistancesVSGenerations() {
	if (ImPlot::BeginPlot("DistancesVSGenerations", plotSize, defaultPlotFlags)) {
		ImPlot::SetupAxis(ImAxis_X1, "Generation", defaultPlotAxisFlags);
		ImPlot::SetupAxis(ImAxis_Y1, "Distance", defaultPlotAxisFlags);
		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);

		MGEA::DistanceData& distanceData = plotData.distanceData;
		if (0 == distanceData.maximumsOfGenerations.size()) {
			ImPlot::EndPlot();
			return;
		}
		auto & numberOfGenerationsDouble(distanceData.numberOfGenerationsDouble);
		auto & minimumOfGenerations(distanceData.minimumOfGenerations);
		auto & maximumOfGenerations(distanceData.maximumOfGenerations);
		auto & diffOfGenerations(distanceData.diffOfGenerations);
		auto & minimumsOfGenerations(distanceData.minimumsOfGenerations);
		auto & maximumsOfGenerations(distanceData.maximumsOfGenerations);
		auto & meansOfGenerations(distanceData.meansOfGenerations);
		auto & numberOfGenerationsInt(distanceData.numberOfGenerationsInt);

		ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, numberOfGenerationsDouble, ImPlotCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, minimumOfGenerations - 0.1 * diffOfGenerations, maximumOfGenerations + 0.1 * diffOfGenerations, ImPlotCond_Always);
		ImPlot::SetupFinish();

		ImPlot::PlotShadedG("All generations distance interval", fitnessGetter, &minimumsOfGenerations, fitnessGetter, &maximumsOfGenerations, numberOfGenerationsInt);
		ImPlot::PlotLine("All generations mean distance", &meansOfGenerations[0], numberOfGenerationsInt, 1.0, 0.0, defaultPlotLineFlags);

		ImPlot::EndPlot();
	}
}

void GUIStateDrawer::drawDistancesVSIndividuals() {
	return;
	if (ImPlot::BeginPlot("drawDistancesVSIndividuals", plotSize, defaultPlotFlags)) {
		ImPlot::SetupAxis(ImAxis_X1, "Individual", defaultPlotAxisFlags);
		ImPlot::SetupAxis(ImAxis_Y1, "Distance", defaultPlotAxisFlags);
		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);

		MGEA::DistanceData& distanceData = plotData.distanceData;
		if (0 == distanceData.maximumsOfGenerations.size()) {
			ImPlot::EndPlot();
			return;
		}
		auto& numberOfGenerationsDouble(distanceData.numberOfGenerationsDouble);
		auto& minimumOfGenerations(distanceData.minimumOfGenerations);
		auto& maximumOfGenerations(distanceData.maximumOfGenerations);
		auto& diffOfGenerations(distanceData.diffOfGenerations);
		auto& minimumsOfGenerations(distanceData.minimumsOfGenerations);
		auto& maximumsOfGenerations(distanceData.maximumsOfGenerations);
		auto& meansOfGenerations(distanceData.meansOfGenerations);
		auto& numberOfGenerationsInt(distanceData.numberOfGenerationsInt);

		auto& minimumOfIndividuals(distanceData.minimumOfIndividuals);
		auto& maximumOfIndividuals(distanceData.maximumOfIndividuals);
		auto& diffOfIndividuals(distanceData.diffOfIndividuals);
		auto& minimumsOfIndividuals(distanceData.minimumsOfIndividuals);
		auto& maximumsOfIndividuals(distanceData.maximumsOfIndividuals);
		std::size_t numberOfIndividualsSizeT(distanceData.numberOfIndividuals);
		double numberOfIndividualsDouble(static_cast<double>(distanceData.numberOfIndividuals));

		ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, numberOfIndividualsDouble, ImPlotCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, minimumOfIndividuals - 0.1 * diffOfIndividuals, maximumOfIndividuals + 0.1 * diffOfIndividuals, ImPlotCond_Always);
		ImPlot::SetupFinish();

		ImPlot::PlotShadedG("All indivduals distance interval", fitnessGetter, &minimumsOfIndividuals, fitnessGetter, &maximumsOfIndividuals, numberOfIndividualsSizeT);

		ImPlot::EndPlot();
	}
}
