#include "GUI/GUIStateDrawer.h"

#include "GUI/Initialisation.h"

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

GUIStateDrawer::GUIStateDrawer() : generationIndex(0)
								 , hideLog(false)
								 , hidePlotSelection(false) {
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

	plotMap.emplace(std::make_pair("Fitness vs Individuals", std::make_pair(false, std::bind_front(&GUIStateDrawer::drawFitnessVSIndividualsPlot, this))));
	plotMap.emplace(std::make_pair("Fitness vs Generations", std::make_pair(false, std::bind_front(&GUIStateDrawer::drawFitnessVSGenerationsPlot, this))));
	plotMap.emplace(std::make_pair("Variation Success at Generation", std::make_pair(false, [](GUIState &){})));
	plotMap.emplace(std::make_pair("Variation Success Throughout Genealogy", std::make_pair(false, [](GUIState &) {})));
}

void GUIStateDrawer::draw(GUIState & state) {
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
	ImVec2 boxSize(windowWidth / 16, windowHeight / 9);
	ImVec2 halfBoxSize(boxSize.x / 2, boxSize.y / 2);
	gridSize = boxSize;
	halfGridSize = halfBoxSize;

	plotSize = gridSize;
	defaultPlotStyle = ImPlot::GetStyle();
	defaultPlotStyle.PlotPadding = ImVec2(1, 1);
	defaultPlotStyle.LabelPadding = ImVec2(1, 1);
	defaultPlotStyle.PlotDefaultSize = plotSize;
	defaultPlotStyle.PlotMinSize = plotSize;
	
	ImGui::GetStyle() = defaultPlotWindowStyle;
	ImPlot::GetStyle() = defaultPlotStyle;

	auto generationProgressPlotDatum = state.generationProgressPlotDatum(generationIndex);
	auto genealogyProgressDatum = state.genealogyProgressDatum();
	//auto generationFitnessPlotDatum = state.generationFitnessPlotDatum(generationIndex);
	//auto genealogyFitnessPlotDatum = state.genealogyFitnessPlotDatum(generationIndex);

	bool genealogyProgressDataReady = genealogyProgressDatum.has_value();
	bool progressPlotDataReady = generationProgressPlotDatum.has_value();
	//bool fitnessPlotDataReady = generationFitnessPlotDatum.has_value()
	//						  and genealogyFitnessPlotDatum.has_value();

	ImGui::SetNextWindowPos(ImVec2(offset.x, offset.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(16 * boxSize.x, 9 * boxSize.y), ImGuiCond_Always);
	ImGui::Begin("##Main", NULL, defaultPlotWindowFlags);

	ImGui::Dummy(ImVec2(12 * boxSize.x, boxSize.y));
	ImGui::SameLine();

	bool startButtonClicked = ImGui::Button("Start", ImVec2(boxSize.x, boxSize.y));
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to start simulation");
	}
	ImGui::SameLine();

	bool pauseButtonClicked = ImGui::Button("Pause", ImVec2(boxSize.x, boxSize.y));
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to pause simulation");
	}
	ImGui::SameLine();

	bool stopButtonClicked = ImGui::Button("Stop", ImVec2(boxSize.x, boxSize.y));
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to stop simulation");
	}
	ImGui::SameLine();

	bool exitButtonClicked = ImGui::Button("Exit", ImVec2(boxSize.x, boxSize.y));
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to stop simulation and quit application");
	}

	ImGui::NewLine();

	bool selectAllGenerationsButtonClicked = ImGui::Button("Select\nAll\nGenerations", ImVec2(boxSize.x, boxSize.y));
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to select all generations for generation-dependent plots");
	}
	ImGui::SameLine();

	bool selectLastGenerationButtonClicked = ImGui::Button("Select\nLast\nGeneration", ImVec2(boxSize.x, boxSize.y));
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press to select last generation for generation-dependent plots");
	}
	ImGui::SameLine();

	ImGui::Dummy(ImVec2(halfBoxSize.x, halfBoxSize.y));
	ImGui::SameLine();

	ImGui::BeginGroup();
	float progress(0.0);
	if (progressPlotDataReady) {
		float failed = static_cast<float>(generationProgressPlotDatum->failed);
		float evaluated = static_cast<float>(generationProgressPlotDatum->evaluated);
		float total = static_cast<float>(generationProgressPlotDatum->total);
		if (0.0 != total) {
			progress = (failed + evaluated) / total;
		}
	}
	std::string progressStr = std::format("{}%", progress);
	ImGui::ProgressBar(progress, ImVec2(15 * boxSize.x, halfBoxSize.y), progressStr.c_str());

	ImGui::NewLine();

	int numberOfGenerations = 0;
	if (genealogyProgressDataReady) {
		numberOfGenerations = static_cast<int>(genealogyProgressDatum->numberOfGenerations);
		if (selectLastGenerationButtonClicked) {
			generationIndex = static_cast<int>(genealogyProgressDatum->numberOfGenerations);
		}
	}
	ImGui::SetNextWindowSize(ImVec2(12 * boxSize.x, halfBoxSize.y));
	ImGui::SliderInt("##Generation", &generationIndex, 0, numberOfGenerations);
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Select target generation for generation-dependent plots");
	}
	ImGui::EndGroup();

	ImGui::NewLine();

	if (!hideLog) {
		static float wrapWidth = 1.0;
		wrapWidth = static_cast<float>(5.5 * boxSize.x);
		std::string logText;
		for (auto & log : guiLoggerPtr->getLogs()) {
			logText += log;
		}
		ImVec2 textSize(6 * boxSize.x, 6 * boxSize.y);
		ImGui::SetNextItemWidth(textSize.x);
		ImGuiWindowFlags logWindowFlags = defaultPlotWindowFlags
			| ImGuiWindowFlags_HorizontalScrollbar;
		ImGui::BeginChild("##LogWindow", textSize, false, logWindowFlags);
		ImGui::Text(logText.c_str(), wrapWidth);
		{
			auto * drawList = ImGui::GetWindowDrawList();
			auto textRectMin = ImGui::GetItemRectMin();
			auto textRectMax = ImGui::GetItemRectMax();
			textRectMax = ImVec2(textRectMin.x + textSize.x, textRectMin.y + textSize.y);
			drawList->AddRect(textRectMin, textRectMax, IM_COL32(128, 128, 128, 128));
		}
		ImGui::EndChild();

		ImGui::SameLine();
	}

	std::string hideLogText = hideLog ? ">##HideLog" : "<##HideLog";
	bool hideLogClicked = ImGui::Button(hideLogText.c_str(), ImVec2(halfBoxSize.x, 6 * boxSize.y));
	if (ImGui::IsItemHovered()) {
		if (hideLog) {
			ImGui::SetTooltip("Press to show log window");
		} else {
			ImGui::SetTooltip("Press to hide log window");
		}
	}
	if (hideLogClicked) {
		hideLog = !hideLog;
	}

	ImGui::SameLine();

	if (!hidePlotSelection) {
		ImGuiWindowFlags plotSelectionWindowFlags = defaultPlotWindowFlags;
		ImGui::BeginChild("##PlotSelectionWindow", ImVec2(3.5 * boxSize.x, 6 * boxSize.y), false, plotSelectionWindowFlags);
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
	}

	std::string hidePlotSelectionText = hidePlotSelection ? ">##HidePlotSelection" : "<##HidePlotSelection";
	bool hidePlotSelectionClicked = ImGui::Button(hidePlotSelectionText.c_str(), ImVec2(halfBoxSize.x, 6 * boxSize.y));
	if (ImGui::IsItemHovered()) {
		if (hidePlotSelection) {
			ImGui::SetTooltip("Press to show plot selection window");
		} else {
			ImGui::SetTooltip("Press to hide plot selection window");
		}
	}
	if (hidePlotSelectionClicked) {
		hidePlotSelection = !hidePlotSelection;
	}

	ImGuiWindowFlags plotWindowFlags = defaultPlotWindowFlags;
	ImGui::BeginChild("##PlotWindow", ImVec2(6 * boxSize.x, 6 * boxSize.y), false, plotWindowFlags);
	auto selected = getSelectedPlotFunctions(plotMap);
	for (auto & plotName : selected) {
		auto & plotPair = plotMap.at(plotName);
		auto & plotFunction = plotPair.second;
		plotFunction(state);
		ImGui::SameLine();
	}
	ImGui::EndChild();
	ImGui::End();
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
	if (0 == numGenerations) {
		return;
	}
	std::size_t lastGeneration(getLastGeneration(state));
	auto generationFPD = state.generationFitnessPlotDatum(lastGeneration);
	auto genealogyFPD = state.genealogyFitnessPlotDatum(lastGeneration);
	bool ghostRun = false;
	if (!generationFPD.has_value() or !genealogyFPD.has_value()) {
		ghostRun = true;
	}

	std::vector<Spec::Fitness> minimumOfIndividuals = genealogyFPD->minimumOfIndividuals;
	std::vector<Spec::Fitness> maximumOfIndividuals = genealogyFPD->maximumOfIndividuals;
	std::vector<Spec::Fitness> meanOfIndividuals = genealogyFPD->meanOfIndividuals;
	double numberOfIndividuals = static_cast<double>(genealogyFPD->numberOfIndividuals);

	std::vector<Spec::Fitness> lastGenerationFitnesses = generationFPD->fitnesses;
	Spec::Fitness lastGenerationMinimumFitness = generationFPD->minimum;
	Spec::Fitness lastGenerationMaximumFitness = generationFPD->maximum;
	int lastGenerationIndividualCount = static_cast<int>(generationFPD->count);
	Spec::Fitness lastGenerationDiff = lastGenerationMaximumFitness - lastGenerationMinimumFitness;

	if (ImPlot::BeginPlot("All Generation Plots", plotSize, defaultPlotFlags)) {
		ImPlot::SetupAxis(ImAxis_X1, "Individual", defaultPlotAxisFlags);
		ImPlot::SetupAxis(ImAxis_Y1, "Fitness", defaultPlotAxisFlags);
		ImPlot::SetupLegend(ImPlotLocation_SouthWest);

		ImPlot::SetupAxisLimits(ImAxis_X1, 0, numberOfIndividuals);
		ImPlot::SetupAxisLimits(ImAxis_Y1, lastGenerationMinimumFitness - 3 * lastGenerationDiff, lastGenerationMaximumFitness + lastGenerationDiff);
		ImPlot::SetupFinish();

		ImPlot::PlotShadedG("All generations fitness interval", fitnessGetter, &meanOfIndividuals, fitnessGetter, &maximumOfIndividuals, numberOfIndividuals);
		ImPlot::PlotLine("Last Generation Fitness", &lastGenerationFitnesses[0], lastGenerationIndividualCount, 1.0, 0.0, defaultPlotLineFlags);

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
		ImPlot::SetupLegend(ImPlotLocation_SouthEast);

		ImPlot::SetupAxisLimits(ImAxis_X1, 0, numberOfGenerations);
		ImPlot::SetupAxisLimits(ImAxis_Y1, lastGenerationMinimumFitness - 3 * lastGenerationDiff, lastGenerationMaximumFitness + lastGenerationDiff);
		ImPlot::SetupFinish();

		ImPlot::PlotShadedG("All generations fitness interval", fitnessGetter, &minimumOfGenerations, fitnessGetter, &maximumOfGenerations, numberOfGenerations);
		ImPlot::PlotLine("All generations mean fitness", &meanOfGenerations[0], numberOfGenerations, 1.0, 0.0, defaultPlotLineFlags);

		ImPlot::EndPlot();
	}
	
}
