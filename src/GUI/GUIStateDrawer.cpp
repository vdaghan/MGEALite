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
	guiLoggerPtr->addToLogger("MGEALogger", guiLoggerPtr);
}

void GUIStateDrawer::initialise(GLFWwindow * w) {
	window = w;
	defaultPlotWindowStyle = ImGui::GetStyle();
	defaultPlotWindowStyle.WindowPadding = ImVec2(0, 0);
	defaultPlotWindowStyle.WindowBorderSize = 0.0;
	defaultPlotWindowStyle.ChildBorderSize = 0.0;
	defaultPlotWindowStyle.FramePadding = ImVec2(0, 0);
	defaultPlotWindowStyle.FrameBorderSize = 0.0;
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

	//plotMap.emplace(std::make_pair("Fitness vs Individuals", std::make_pair(true, std::bind_front(&GUIStateDrawer::drawFitnessVSIndividualsPlot, this))));
	//plotMap.emplace(std::make_pair("Fitness vs Generations", std::make_pair(true, std::bind_front(&GUIStateDrawer::drawFitnessVSGenerationsPlot, this))));
	//plotMap.emplace(std::make_pair("VariationSuccess vs Variations", std::make_pair(true, std::bind_front(&GUIStateDrawer::drawVariationVSVariationStatistics, this))));
	plotMap.emplace(std::make_pair("Distances vs Generations", std::make_pair(true, [&]() {drawDistancesVSGenerations(); })));
}

void GUIStateDrawer::draw() {
	if (glfwWindowShouldClose(window)) {
		exitFlag.setAndCall("stop", FlagSetType::True);
		exitFlag.setAndCall("exit", FlagSetType::True);
	}

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
	gridSize = ImVec2(windowWidth / 16, windowHeight / 9);
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
	drawTopRow();
	ImGui::NewLine();
	drawProgressbarAndSlider();
	ImGui::NewLine();
	drawLogsOrPlots();
	ImGui::End();
}

void GUIStateDrawer::drawTopRow() {
	ImGui::Dummy(ImVec2(12 * gridSize.x, gridSize.y));
	ImGui::SameLine();

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
	bool trackLastGenerationButtonClicked = ImGui::Button("Track\nLast\nGeneration", ImVec2(gridSize.x, gridSize.y));
	ImGui::PopStyleColor(2);
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to select last generation for generation-dependent plots");
	}
	if (trackLastGenerationButtonClicked) {
		lastGenerationSelected = not lastGenerationSelected;
	}
	ImGui::SameLine();

	ImGui::Dummy(ImVec2(halfGridSize.x, halfGridSize.y));
	ImGui::SameLine();

	ImGui::BeginGroup();
	auto fraction(plotData.progressbarData.fraction());
	auto progressText(plotData.progressbarData.text());
	ImGui::ProgressBar(fraction, ImVec2(14.5 * gridSize.x, halfGridSize.y), progressText.c_str());

	ImGui::NewLine();

	auto& generationIndex(plotData.sliderData.generationIndex(lastGenerationSelected));
	auto currentGeneration(plotData.sliderData.currentGeneration());
	ImGui::PushItemWidth(14.5 * gridSize.x);
	bool sliderValueChanged = ImGui::SliderInt("##Generation", &generationIndex, 0, currentGeneration);
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Select target generation for generation-dependent plots");
	}
	ImGui::EndGroup();
}

void GUIStateDrawer::drawLogsOrPlots() {
	if (LogOrPlot::Log == logOrPlot) {
		static float wrapWidth = 1.0;
		wrapWidth = static_cast<float>(5.5 * gridSize.x);
		std::string logText;
		std::size_t totalLength(0);
		auto logs = guiLoggerPtr->getLogs();
		for (auto it(logs.rbegin()); it != logs.rend(); ++it) {
			logText += *it;
		}
		ImVec2 textSize(15 * gridSize.x, 6 * gridSize.y);
		ImGuiWindowFlags logWindowFlags = defaultPlotWindowFlags
			| ImGuiWindowFlags_HorizontalScrollbar;
		ImGuiInputTextFlags loggerFlags =
			ImGuiInputTextFlags_ReadOnly
			| ImGuiInputTextFlags_NoUndoRedo;
		ImGui::InputTextMultiline("##Logger", &logText, textSize, loggerFlags);
		ImGui::SetScrollHereY(1.0f);
		//{
		//	auto * drawList = ImGui::GetWindowDrawList();
		//	auto textRectMin = ImGui::GetItemRectMin();
		//	auto textRectMax = ImGui::GetItemRectMax();
		//	textRectMax = ImVec2(textRectMin.x + textSize.x, textRectMin.y + textSize.y);
		//	drawList->AddRect(textRectMin, textRectMax, IM_COL32(128, 128, 128, 128));
		//}

		ImGui::SameLine();
	}

	std::string hideLogText = (LogOrPlot::Log == logOrPlot) ? "<##HideLog" : ">##HideLog";
	bool hideLogClicked = ImGui::Button(hideLogText.c_str(), ImVec2(gridSize.x, 6 * gridSize.y));
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

	ImGui::SameLine();

	if (LogOrPlot::Plot == logOrPlot) {
		ImGuiWindowFlags plotSelectionWindowFlags = defaultPlotWindowFlags;
		ImGui::BeginChild("##PlotSelectionWindow", ImVec2(3 * gridSize.x, 6 * gridSize.y), false, plotSelectionWindowFlags);
		ImGui::BeginGroup();
		for (auto & plotNamePair : plotMap) {
			auto & plotName = plotNamePair.first;
			auto & plotBoolFunction = plotNamePair.second;
			auto & plotBool = plotBoolFunction.first;
			ImGui::Selectable(plotName.c_str(), &plotBool);
		}
		ImGui::EndGroup();
		ImGui::EndChild();

		ImGui::SameLine();

		ImGuiWindowFlags plotWindowFlags = defaultPlotWindowFlags;
		ImGui::BeginChild("##PlotWindow", ImVec2(12 * gridSize.x, 6 * gridSize.y), false, plotWindowFlags);
		ImVec2 ImGui::GetCursorPos();
		int col(0);
		int row(0);
		auto selected = getSelectedPlotFunctions(plotMap);
		for (auto & plotName : selected) {
			auto & plotPair = plotMap.at(plotName);
			auto & plotFunction = plotPair.second;
			ImVec2 cursorPos(col * plotSize.x, row * plotSize.y);
			ImGui::SetCursorPos(cursorPos);
			//plotFunction(state);
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
	std::vector<Spec::Fitness> * userDataAsVector = static_cast<std::vector<Spec::Fitness> *>(user_data);
	return ImPlotPoint(idx, userDataAsVector->at(idx));
};

void GUIStateDrawer::drawFitnessVSIndividualsPlot() {
	//if (ImPlot::BeginPlot("FitnessVSIndividuals", plotSize, defaultPlotFlags)) {
	//	ImPlot::SetupAxis(ImAxis_X1, "Individual", defaultPlotAxisFlags);
	//	ImPlot::SetupAxis(ImAxis_Y1, "Fitness", defaultPlotAxisFlags);
	//	ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);

	//	std::optional<DEvA::EAStatistics<Spec>> eaStatistics = state.getEAStatistics();
	//	if (!eaStatistics or eaStatistics->fitnesses.empty()) {
	//		ImPlot::EndPlot();
	//		return;
	//	}
	//	auto & fitnessesList = eaStatistics->fitnesses;
	//	std::vector<Spec::Fitness> fitnessesVector(fitnessesList.begin(), fitnessesList.end());
	//	double lastGenerationMinimumFitness(*std::min_element(fitnessesList.begin(), fitnessesList.end()));
	//	double lastGenerationMaximumFitness(*std::max_element(fitnessesList.begin(), fitnessesList.end()));
	//	double lastGenerationDiff(lastGenerationMaximumFitness - lastGenerationMinimumFitness);
	//	int numberOfIndividualsInt(static_cast<int>(fitnessesVector.size()));
	//	double numberOfIndividualsDouble(static_cast<double>(numberOfIndividualsInt));

	//	ImPlot::SetupAxisLimits(ImAxis_X1, 0, numberOfIndividualsDouble, ImPlotCond_Always);
	//	ImPlot::SetupAxisLimits(ImAxis_Y1, lastGenerationMinimumFitness - 0.1 * lastGenerationDiff, lastGenerationMaximumFitness + 0.1 * lastGenerationDiff, ImPlotCond_Always);
	//	ImPlot::SetupFinish();

	//	ImPlot::PlotLine("Last Generation Fitness", &fitnessesVector[0], numberOfIndividualsInt, 1.0, 0.0, defaultPlotLineFlags);
	//	ImPlot::EndPlot();
	//}
}

void GUIStateDrawer::drawFitnessVSGenerationsPlot() {
	//if (ImPlot::BeginPlot("Fitness Evolution", plotSize, defaultPlotFlags)) {
	//	ImPlot::SetupAxis(ImAxis_X1, "Generation", defaultPlotAxisFlags);
	//	ImPlot::SetupAxis(ImAxis_Y1, "Fitness", defaultPlotAxisFlags);
	//	ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);

	//	std::optional<DEvA::EAStatisticsHistory<Spec>> eaStatisticsHistory = state.getEAStatisticsHistory();
	//	if (!eaStatisticsHistory or eaStatisticsHistory->empty()) {
	//		ImPlot::EndPlot();
	//		return;
	//	}
	//	std::vector<double> minimumOfGenerations;
	//	std::vector<double> maximumOfGenerations;
	//	std::vector<double> meanOfGenerations;
	//	double lastMinimumFitness{};
	//	double lastMaximumFitness{};
	//	for (auto& eaStatistics : *eaStatisticsHistory) {
	//		auto& fitnessesList = eaStatistics.fitnesses;
	//		if (fitnessesList.empty()) {
	//			continue;
	//		}
	//		double numberOfIndividuals(static_cast<double>(fitnessesList.size()));
	//		double minimumFitness(*std::min_element(fitnessesList.begin(), fitnessesList.end()));
	//		double maximumFitness(*std::max_element(fitnessesList.begin(), fitnessesList.end()));
	//		double totalFitness(std::accumulate(fitnessesList.begin(), fitnessesList.end(), 0.0));
	//		double meanFitness(totalFitness / numberOfIndividuals);
	//		minimumOfGenerations.push_back(minimumFitness);
	//		maximumOfGenerations.push_back(maximumFitness);
	//		meanOfGenerations.push_back(meanFitness);
	//		lastMinimumFitness = minimumFitness;
	//		lastMaximumFitness = maximumFitness;
	//	}
	//	if (minimumOfGenerations.empty()) {
	//		ImPlot::EndPlot();
	//		return;
	//	}
	//	double lastDiff(lastMaximumFitness - lastMinimumFitness);
	//	int numberOfGenerationsInt(static_cast<int>(eaStatisticsHistory->size()));
	//	double numberOfGenerationsDouble(static_cast<double>(eaStatisticsHistory->size()));
	//	ImPlot::SetupAxisLimits(ImAxis_X1, 0, numberOfGenerationsDouble, ImPlotCond_Always);
	//	ImPlot::SetupAxisLimits(ImAxis_Y1, lastMinimumFitness - lastDiff, lastMaximumFitness + lastDiff, ImPlotCond_Always);
	//	ImPlot::SetupFinish();

	//	ImPlot::PlotShadedG("All generations fitness interval", fitnessGetter, &minimumOfGenerations, fitnessGetter, &maximumOfGenerations, numberOfGenerationsInt);
	//	ImPlot::PlotLine("All generations mean fitness", &meanOfGenerations[0], numberOfGenerationsInt, 1.0, 0.0, defaultPlotLineFlags);

	//	ImPlot::EndPlot();
	//}
}

void GUIStateDrawer::drawVariationVSVariationStatistics() {
	//if (ImPlot::BeginPlot("Variation Success", plotSize, defaultPlotFlags)) {
	//	ImPlotAxisFlags axisFlags =
	//		ImPlotAxisFlags_NoGridLines
	//		| ImPlotAxisFlags_NoMenus
	//		| ImPlotAxisFlags_NoSideSwitch
	//		| ImPlotAxisFlags_NoHighlight
	//		| ImPlotAxisFlags_AutoFit;
	//	ImPlot::SetupAxis(ImAxis_X1, "Variation Success", axisFlags);
	//	ImPlot::SetupAxis(ImAxis_Y1, "Variation", axisFlags);
	//	ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);
	//	{
	//		std::optional<DEvA::EAStatistics<Spec>> eaStatistics = state.getEAStatistics();
	//		std::optional<DEvA::EAStatisticsHistory<Spec>> eaStatisticsHistory = state.getEAStatisticsHistory();

	//		if (eaStatistics) {
	//			DEvA::EAProgress eaProgress = eaStatistics->eaProgress;
	//			DEvA::VariationStatisticsMap vSM;
	//			if (generationIndex == eaProgress.currentGeneration) {
	//				vSM = eaStatistics->variationStatisticsMap;
	//			} else if (eaStatisticsHistory and eaStatisticsHistory->size() > generationIndex) {
	//				eaProgress = eaStatisticsHistory->at(generationIndex).eaProgress;
	//				vSM = eaStatisticsHistory->at(generationIndex).variationStatisticsMap;
	//			}

	//			if (!vSM.empty()) {
	//				std::vector<std::string> variationNames;
	//				std::vector<double> successRates;
	//				for (auto & varStatPair : vSM) {
	//					auto & variationName = varStatPair.first;
	//					double successRate = varStatPair.second.successRate();

	//					variationNames.push_back(variationName);
	//					successRates.push_back(successRate);
	//				}
	//				std::vector<double> tickValues;
	//				for (std::size_t i(0); i != variationNames.size(); ++i) {
	//					tickValues.push_back(double(i));
	//				}
	//				ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 1.0, ImPlotCond_Always);
	//				ImPlot::SetupAxisTicks(ImAxis_Y1, tickValues, variationNames, false);
	//				ImPlot::SetupAxisLimits(ImAxis_Y1, -1.0, double(tickValues.size()), ImPlotCond_Always);
	//				ImPlot::SetupFinish();

	//				ImPlot::PlotBars("Variation Success in Generation", &successRates[0], successRates.size(), 1.0, 0.0, ImPlotBarsFlags_Horizontal);
	//			}
	//		}
	//	}
	//	ImPlot::EndPlot();
	//}
}

void GUIStateDrawer::drawDistancesVSGenerations() {
	if (ImPlot::BeginPlot("DistancesVSGenerations", plotSize, defaultPlotFlags)) {
		ImPlot::SetupAxis(ImAxis_X1, "Generation", defaultPlotAxisFlags);
		ImPlot::SetupAxis(ImAxis_Y1, "Distance", defaultPlotAxisFlags);
		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);

		DistanceData& distanceData = plotData.distanceData;
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
		ImPlot::SetupAxisLimits(ImAxis_Y1, minimumOfGenerations - diffOfGenerations, maximumOfGenerations + diffOfGenerations, ImPlotCond_Always);
		ImPlot::SetupFinish();

		ImPlot::PlotShadedG("All generations distance interval", fitnessGetter, &minimumsOfGenerations, fitnessGetter, &maximumsOfGenerations, numberOfGenerationsInt);
		ImPlot::PlotLine("All generations mean distance", &meansOfGenerations[0], numberOfGenerationsInt, 1.0, 0.0, defaultPlotLineFlags);

		ImPlot::EndPlot();
	}
}
