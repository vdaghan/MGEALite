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

void GUIStateDrawer::initialise() {
	defaultPlotWindowStyle = ImGui::GetStyle();
	defaultPlotWindowStyle.WindowPadding = ImVec2(0, 0);
	defaultPlotWindowStyle.WindowBorderSize = 0.0;
	defaultPlotWindowStyle.ChildBorderSize = 0.0;
	defaultPlotWindowStyle.FramePadding = ImVec2(0, 0);
	defaultPlotWindowStyle.FrameBorderSize = 0.0;
	defaultPlotWindowStyle.ItemSpacing = ImVec2(0, 0);
	defaultPlotWindowStyle.ItemInnerSpacing = ImVec2(5, 5);

	defaultPlotWindowFlags = ImGuiWindowFlags_NoTitleBar
						   | ImGuiWindowFlags_NoResize
						   | ImGuiWindowFlags_NoMove
						   | ImGuiWindowFlags_NoCollapse
						   | ImGuiWindowFlags_NoSavedSettings;

	defaultPlotStyle = ImPlot::GetStyle();
	defaultPlotStyle.PlotPadding = ImVec2(1, 1);
	defaultPlotStyle.LabelPadding = ImVec2(1, 1);
	defaultPlotStyle.PlotDefaultSize = ImVec2(480 - 10, 270 - 10);
	defaultPlotStyle.PlotMinSize = ImVec2(480 - 10, 270 - 10);

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
}

void GUIStateDrawer::draw(GUIState & state) {
	static bool initialised(false);
	if (!initialised) {
		initialise();
		initialised = true;
	}
	
	ImGui::GetStyle() = defaultPlotWindowStyle;
	ImPlot::GetStyle() = defaultPlotStyle;

	std::size_t generations(getNumGenerations(state));
	std::size_t lastGeneration(getLastGeneration(state));

	auto generationProgressPlotDatum = state.generationProgressPlotDatum(lastGeneration);
	auto generationFitnessPlotDatum = state.generationFitnessPlotDatum(lastGeneration);
	auto genealogyFitnessPlotDatum = state.genealogyFitnessPlotDatum(lastGeneration);

	bool progressPlotDataReady = generationProgressPlotDatum.has_value();
	bool fitnessPlotDataReady = generationFitnessPlotDatum.has_value()
							  and genealogyFitnessPlotDatum.has_value();

	ImVec2 boxSize(120, 120);
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(15 * boxSize.x, 8 * boxSize.y), ImGuiCond_Always);
	ImGui::Begin("##Main", NULL, defaultPlotWindowFlags);

	ImGui::Dummy(ImVec2(11 * boxSize.x, boxSize.y));
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
	ImGui::ProgressBar(progress, ImVec2(15 * boxSize.x, boxSize.y), progressStr.c_str());

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

	static int generationIndex = 0;
	static int numberOfGenerations = -1;
	if (fitnessPlotDataReady) {
		numberOfGenerations = static_cast<int>(genealogyFitnessPlotDatum->numberOfGenerations);
	}
	ImGui::VSliderInt("Generation:", ImVec2(13 * boxSize.x, boxSize.y), &generationIndex, -1, numberOfGenerations);
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Select target generation for generation-dependent plots");
	}

	ImGui::NewLine();

	static float wrapWidth = 1.0;
	wrapWidth = static_cast<float>(6 * boxSize.x);
	std::string logText("Hello, MGEALight!");
	ImVec2 textSize(6 * boxSize.x, 5 * boxSize.y);
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

	ImGuiWindowFlags plotSelectionWindowFlags = defaultPlotWindowFlags;
	ImGui::BeginChild("##PlotSelectionWindow", ImVec2(4 * boxSize.x, 3 * boxSize.y), false, plotSelectionWindowFlags);
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
	ImGui::BeginChild("##PlotWindow", ImVec2(6 * boxSize.x, 5 * boxSize.y), false, plotWindowFlags);
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
	if (!generationFPD.has_value() or !genealogyFPD.has_value()) {
		return;
	}

	auto & minimumOfIndividuals = genealogyFPD->minimumOfIndividuals;
	auto & maximumOfIndividuals = genealogyFPD->maximumOfIndividuals;
	auto & meanOfIndividuals = genealogyFPD->meanOfIndividuals;
	auto & numberOfIndividuals = genealogyFPD->numberOfIndividuals;

	auto & lastGenerationFitnesses = generationFPD->fitnesses;
	auto & lastGenerationMinimumFitness = generationFPD->minimum;
	auto & lastGenerationMaximumFitness = generationFPD->maximum;
	auto & lastGenerationIndividualCount = generationFPD->count;
	auto lastGenerationDiff = lastGenerationMaximumFitness - lastGenerationMinimumFitness;

	if (ImPlot::BeginPlot("All Generation Plots", ImVec2(0, 0), defaultPlotFlags)) {
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
	auto & numberOfGenerations = genealogyFPD->numberOfGenerations;

	auto & lastGenerationMinimumFitness = generationFPD->minimum;
	auto & lastGenerationMaximumFitness = generationFPD->maximum;
	auto lastGenerationDiff = lastGenerationMaximumFitness - lastGenerationMinimumFitness;

	if (ImPlot::BeginPlot("Fitness Evolution", ImVec2(0, 0), defaultPlotFlags)) {
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
