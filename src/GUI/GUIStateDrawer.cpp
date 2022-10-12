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
	defaultPlotWindowStyle.ItemSpacing = ImVec2(0, 0);
	defaultPlotWindowStyle.ItemInnerSpacing = ImVec2(5, 5);
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

	plotMap.emplace(std::make_pair("Fitness vs Individuals", std::make_pair(true, std::bind_front(&GUIStateDrawer::drawFitnessVSIndividualsPlot, this))));
	plotMap.emplace(std::make_pair("Fitness vs Generations", std::make_pair(true, std::bind_front(&GUIStateDrawer::drawFitnessVSGenerationsPlot, this))));
	plotMap.emplace(std::make_pair("VariationSuccess vs Variations", std::make_pair(true, std::bind_front(&GUIStateDrawer::drawVariationVSVariationStatistics, this))));
}

void GUIStateDrawer::draw(GUIState & state) {
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
	defaultPlotStyle.PlotPadding = ImVec2(1, 1);
	defaultPlotStyle.LabelPadding = ImVec2(1, 1);
	defaultPlotStyle.PlotDefaultSize = plotSize;
	defaultPlotStyle.PlotMinSize = plotSize;
	
	ImGui::GetStyle() = defaultPlotWindowStyle;
	ImPlot::GetStyle() = defaultPlotStyle;

	ImGui::SetNextWindowPos(ImVec2(offset.x, offset.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(16 * gridSize.x, 9 * gridSize.y), ImGuiCond_Always);
	ImGui::Begin("##Main", NULL, defaultPlotWindowFlags);
	drawTopRow(state);
	ImGui::NewLine();
	drawProgressbarAndSlider(state);
	ImGui::NewLine();
	drawLogsOrPlots(state);
	ImGui::End();
}

void GUIStateDrawer::drawTopRow(GUIState &) {
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

void GUIStateDrawer::drawProgressbarAndSlider(GUIState & state) {
	std::optional<DEvA::EAStatistics> eaStatistics = state.getEAStatistics();

	bool progressbarDataReady = eaStatistics.has_value();
	bool sliderDataReady = eaStatistics.has_value();
	DEvA::EAProgress eaProgress = progressbarDataReady ? eaStatistics->eaProgress : DEvA::EAProgress{};
	DEvA::VariationStatisticsMap vSM = progressbarDataReady ? eaStatistics->variationStatisticsMap : DEvA::VariationStatisticsMap{};

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
	float progress(0.0);
	if (progressbarDataReady) {
		float processed;
		if (eaProgress.numberOfTransformedIndividualsInGeneration != eaProgress.numberOfIndividualsInGeneration) {
			processed = static_cast<float>(eaProgress.numberOfTransformedIndividualsInGeneration);
		} else {
			processed = static_cast<float>(eaProgress.numberOfEvaluatedIndividualsInGeneration);
		} 
		float total = static_cast<float>(eaProgress.numberOfIndividualsInGeneration);
		if (0.0 != total) {
			progress = processed / total;
		}
	}
	std::string progressStr = std::format("{:.2f}%", 100.0 * progress);
	ImGui::ProgressBar(progress, ImVec2(14.5 * gridSize.x, halfGridSize.y), progressStr.c_str());

	ImGui::NewLine();

	int generationIndexInt = static_cast<int>(generationIndex);
	int currentGeneration = 0;
	if (sliderDataReady) {
		currentGeneration = static_cast<int>(eaProgress.currentGeneration);
		if (lastGenerationSelected) {
			generationIndexInt = currentGeneration;
		}
	}
	ImGui::PushItemWidth(14.5 * gridSize.x);
	bool sliderValueChanged = ImGui::SliderInt("##Generation", &generationIndexInt, 0, currentGeneration);
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Select target generation for generation-dependent plots");
	}
	if (sliderValueChanged) {
		generationIndex = static_cast<std::size_t>(generationIndexInt);
	}
	ImGui::EndGroup();
}

void GUIStateDrawer::drawLogsOrPlots(GUIState & state) {
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
		size_t col(0);
		size_t row(0);
		auto selected = getSelectedPlotFunctions(plotMap);
		for (auto & plotName : selected) {
			auto & plotPair = plotMap.at(plotName);
			auto & plotFunction = plotPair.second;
			plotFunction(state);
			++col;
			if (col > 2) {
				col = 0;
				++row;
				ImGui::NewLine();
			} else {
				ImGui::SameLine();
			}
			if (row > 2) {
				break;
			}
		}
		ImGui::EndChild();
	}
}

std::size_t GUIStateDrawer::getNumGenerations(GUIState & state) {
	return state.generations();
}

std::size_t GUIStateDrawer::getLastGeneration(GUIState & state) {
	std::size_t generations = getNumGenerations(state);
	std::size_t lastGeneration(0);
	if (generations > 0) {
		lastGeneration = generations - 1;
	}
	return lastGeneration;
}

ImPlotPoint fitnessGetter(int idx, void * user_data) {
	std::vector<Spec::Fitness> * userDataAsVector = static_cast<std::vector<Spec::Fitness> *>(user_data);
	return ImPlotPoint(idx, userDataAsVector->at(idx));
};

void GUIStateDrawer::drawFitnessVSIndividualsPlot(GUIState & state) {
	std::size_t numGenerations(getNumGenerations(state));
	bool ghostRun = false;
	if (0 == numGenerations) {
		ghostRun = true;
	}
	std::size_t lastGeneration;
	std::optional<GenerationFitnessPlotDatum> generationFPD;
	std::optional<GenealogyFitnessPlotDatum> genealogyFPD;
	if (!ghostRun) {
		lastGeneration = getLastGeneration(state);
		generationFPD = state.generationFitnessPlotDatum(lastGeneration);
		genealogyFPD = state.genealogyFitnessPlotDatum(lastGeneration);
		if (!generationFPD.has_value() or !genealogyFPD.has_value()) {
			ghostRun = true;
		}
	}

	std::vector<Spec::Fitness> minimumOfIndividuals;
	std::vector<Spec::Fitness> maximumOfIndividuals;
	std::vector<Spec::Fitness> meanOfIndividuals;
	double numberOfIndividuals;

	std::vector<Spec::Fitness> lastGenerationFitnesses;
	Spec::Fitness lastGenerationMinimumFitness;
	Spec::Fitness lastGenerationMaximumFitness;
	int lastGenerationIndividualCount;
	Spec::Fitness lastGenerationDiff;

	if (!ghostRun) {
		numberOfIndividuals = static_cast<double>(genealogyFPD->numberOfIndividuals);

		lastGenerationFitnesses = generationFPD->fitnesses;
		lastGenerationMinimumFitness = generationFPD->minimum;
		lastGenerationMaximumFitness = generationFPD->maximum;
		lastGenerationIndividualCount = static_cast<int>(generationFPD->count);
		lastGenerationDiff = lastGenerationMaximumFitness - lastGenerationMinimumFitness;
	}

	if (ImPlot::BeginPlot("All Generation Plots", plotSize, defaultPlotFlags)) {
		ImPlot::SetupAxis(ImAxis_X1, "Individual", defaultPlotAxisFlags);
		ImPlot::SetupAxis(ImAxis_Y1, "Fitness", defaultPlotAxisFlags);
		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);

		if (!ghostRun) {
			ImPlot::SetupAxisLimits(ImAxis_X1, 0, numberOfIndividuals, ImPlotCond_Always);
			ImPlot::SetupAxisLimits(ImAxis_Y1, lastGenerationMinimumFitness - 0.1 * lastGenerationDiff, lastGenerationMaximumFitness + 0.1 * lastGenerationDiff, ImPlotCond_Always);
			ImPlot::SetupFinish();

			ImPlot::PlotLine("Last Generation Fitness", &lastGenerationFitnesses[0], lastGenerationIndividualCount, 1.0, 0.0, defaultPlotLineFlags);
		}

		ImPlot::EndPlot();
	}
}

void GUIStateDrawer::drawFitnessVSGenerationsPlot(GUIState & state) {
	std::size_t numGenerations(getNumGenerations(state));
	if (0 == numGenerations) {
		return;
	}
	std::size_t lastGeneration(getLastGeneration(state));
	auto generationFPD = state.generationFitnessPlotDatum(lastGeneration);
	auto genealogyFPD = state.genealogyFitnessPlotDatum(lastGeneration);
	if (!generationFPD.has_value() or !genealogyFPD.has_value()) {
		return;
	}

	auto & minimumOfGenerations = genealogyFPD->minimumOfGenerations;
	auto & maximumOfGenerations = genealogyFPD->maximumOfGenerations;
	auto & meanOfGenerations = genealogyFPD->meanOfGenerations;
	auto numberOfGenerations = static_cast<double>(genealogyFPD->numberOfGenerations);

	auto & lastGenerationMinimumFitness = generationFPD->minimum;
	auto & lastGenerationMaximumFitness = generationFPD->maximum;
	auto lastGenerationDiff = lastGenerationMaximumFitness - lastGenerationMinimumFitness;

	if (ImPlot::BeginPlot("Fitness Evolution", plotSize, defaultPlotFlags)) {
		ImPlot::SetupAxis(ImAxis_X1, "Generation", defaultPlotAxisFlags);
		ImPlot::SetupAxis(ImAxis_Y1, "Fitness", defaultPlotAxisFlags);
		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);

		ImPlot::SetupAxisLimits(ImAxis_X1, 0, numberOfGenerations, ImPlotCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, lastGenerationMinimumFitness - lastGenerationDiff, lastGenerationMaximumFitness + lastGenerationDiff, ImPlotCond_Always);
		ImPlot::SetupFinish();

		ImPlot::PlotShadedG("All generations fitness interval", fitnessGetter, &minimumOfGenerations, fitnessGetter, &maximumOfGenerations, numberOfGenerations);
		ImPlot::PlotLine("All generations mean fitness", &meanOfGenerations[0], numberOfGenerations, 1.0, 0.0, defaultPlotLineFlags);

		ImPlot::EndPlot();
	}
	
}

void GUIStateDrawer::drawVariationVSVariationStatistics(GUIState & state) {

	if (ImPlot::BeginPlot("Variation Success", plotSize, defaultPlotFlags)) {

		ImPlotAxisFlags xAxisFlags =
			ImPlotAxisFlags_NoGridLines
			| ImPlotAxisFlags_NoMenus
			| ImPlotAxisFlags_NoSideSwitch
			| ImPlotAxisFlags_NoHighlight
			| ImPlotAxisFlags_AutoFit;
		ImPlotAxisFlags yAxisFlags =
			ImPlotAxisFlags_NoGridLines
			| ImPlotAxisFlags_NoMenus
			| ImPlotAxisFlags_NoSideSwitch
			| ImPlotAxisFlags_NoHighlight
			| ImPlotAxisFlags_AutoFit;
		ImPlot::SetupAxis(ImAxis_X1, "Variation Success", xAxisFlags);
		ImPlot::SetupAxis(ImAxis_Y1, "Variation", yAxisFlags);
		ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);
		{
			std::optional<DEvA::EAStatistics> eaStatistics = state.getEAStatistics();
			std::optional<DEvA::EAStatisticsHistory> eaStatisticsHistory = state.getEAStatisticsHistory();

			if (eaStatistics) {
				DEvA::EAProgress eaProgress = eaStatistics->eaProgress;
				DEvA::VariationStatisticsMap vSM;
				if (generationIndex == eaProgress.currentGeneration) {
					vSM = eaStatistics->variationStatisticsMap;
				} else if (eaStatisticsHistory and eaStatisticsHistory->size() > generationIndex) {
					eaProgress = eaStatisticsHistory->at(generationIndex).eaProgress;
					vSM = eaStatisticsHistory->at(generationIndex).variationStatisticsMap;
				}

				if (!vSM.empty()) {
					std::vector<std::string> variationNames;
					std::vector<double> successRates;
					for (auto & varStatPair : vSM) {
						auto & variationName = varStatPair.first;
						double successRate = varStatPair.second.successRate();

						variationNames.push_back(variationName);
						successRates.push_back(successRate);
					}
					std::vector<double> tickValues;
					for (std::size_t i(0); i != variationNames.size(); ++i) {
						tickValues.push_back(double(i));
					}
					ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 1.0, ImPlotCond_Always);
					ImPlot::SetupAxisTicks(ImAxis_Y1, tickValues, variationNames, false);
					ImPlot::SetupAxisLimits(ImAxis_Y1, -1.0, double(tickValues.size()), ImPlotCond_Always);
					ImPlot::SetupFinish();

					ImPlot::PlotBars("Variation Success per VariationFunctor", &successRates[0], successRates.size(), 1.0, 0.0, ImPlotBarsFlags_Horizontal);
					//ImPlot::PlotBars("Variation Success per VariationFunctor", successRates, 1.0, 0.0, ImPlotBarsFlags_Horizontal);
				}
			}
		}
		ImPlot::EndPlot();
	}
}
