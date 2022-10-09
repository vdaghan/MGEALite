#include "GUI/GUIStateDrawer.h"

#include "GUI/Initialisation.h"

#include <algorithm>
#include <limits>

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
}

void GUIStateDrawer::draw(GUIState & state) {
	static bool initialised(false);
	if (!initialised) {
		initialise();
		initialised = true;
	}
	drawPlots(state);
}

void GUIStateDrawer::drawPlots(GUIState & state) {
	ImGui::GetStyle() = defaultPlotWindowStyle;
	ImPlot::GetStyle() = defaultPlotStyle;

	std::size_t generations = state.generations();
	std::size_t lastGeneration(0);
	if (generations > 0) {
		lastGeneration = generations - 1;
	}

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(3 * 480, 270), ImGuiCond_Always);
	ImGui::Begin("##Plots", NULL, defaultPlotWindowFlags);
	auto generationProgressPlotDatum = state.generationProgressPlotDatum(lastGeneration);
	auto generationFitnessPlotDatum = state.generationFitnessPlotDatum(lastGeneration);
	auto genealogyFitnessPlotDatum = state.genealogyFitnessPlotDatum(lastGeneration);

	if (generationProgressPlotDatum and generationFitnessPlotDatum and genealogyFitnessPlotDatum) {
		auto & minimumOfGenerations = genealogyFitnessPlotDatum->minimumOfGenerations;
		auto & maximumOfGenerations = genealogyFitnessPlotDatum->maximumOfGenerations;
		auto & meanOfGenerations = genealogyFitnessPlotDatum->meanOfGenerations;
		auto & numberOfGenerations = genealogyFitnessPlotDatum->numberOfGenerations;

		auto & minimumOfIndividuals = genealogyFitnessPlotDatum->minimumOfIndividuals;
		auto & maximumOfIndividuals = genealogyFitnessPlotDatum->maximumOfIndividuals;
		auto & meanOfIndividuals = genealogyFitnessPlotDatum->meanOfIndividuals;
		auto & numberOfIndividuals = genealogyFitnessPlotDatum->numberOfIndividuals;

		auto & lastGenerationFitnesses = generationFitnessPlotDatum->fitnesses;
		auto & lastGenerationMinimumFitness = generationFitnessPlotDatum->minimum;
		auto & lastGenerationMaximumFitness = generationFitnessPlotDatum->maximum;
		auto & lastGenerationMeanFitness = generationFitnessPlotDatum->mean;
		auto & lastGenerationIndividualCount = generationFitnessPlotDatum->count;
		auto lastGenerationDiff = lastGenerationMaximumFitness - lastGenerationMinimumFitness;
		auto fitnessGetterLambda = [](int idx, void * user_data) -> ImPlotPoint {
			std::vector<Spec::Fitness> * userDataAsVector = static_cast<std::vector<Spec::Fitness> *>(user_data);
			return ImPlotPoint(idx, userDataAsVector->at(idx));
		};
		if (generations > 0) {
			if (ImPlot::BeginPlot("All Generation Plots", ImVec2(0, 0), defaultPlotFlags)) {
				ImPlot::SetupAxis(ImAxis_X1, "Individual", defaultPlotAxisFlags);
				ImPlot::SetupAxis(ImAxis_Y1, "Fitness", defaultPlotAxisFlags);
				ImPlot::SetupLegend(ImPlotLocation_SouthWest);

				ImPlot::SetupAxisLimits(ImAxis_X1, 0, numberOfIndividuals);
				ImPlot::SetupAxisLimits(ImAxis_Y1, lastGenerationMinimumFitness - 3 * lastGenerationDiff, lastGenerationMaximumFitness + lastGenerationDiff);
				ImPlot::SetupFinish();

				ImPlot::PlotShadedG("All generations fitness interval", fitnessGetterLambda, &meanOfIndividuals, fitnessGetterLambda, &maximumOfIndividuals, numberOfIndividuals);
				ImPlot::PlotLine("Last Generation Fitness", &lastGenerationFitnesses[0], lastGenerationIndividualCount, 1.0, 0.0, defaultPlotLineFlags);

				ImPlot::EndPlot();
			}
		}
		ImGui::SameLine();
		if (generations > 0) {
			if (ImPlot::BeginPlot("Fitness Evolution", ImVec2(0, 0), defaultPlotFlags)) {
				ImPlot::SetupAxis(ImAxis_X1, "Generation", defaultPlotAxisFlags);
				ImPlot::SetupAxis(ImAxis_Y1, "Fitness", defaultPlotAxisFlags);
				ImPlot::SetupLegend(ImPlotLocation_SouthEast);

				ImPlot::SetupAxisLimits(ImAxis_X1, 0, numberOfGenerations);
				ImPlot::SetupAxisLimits(ImAxis_Y1, lastGenerationMinimumFitness - 3 * lastGenerationDiff, lastGenerationMaximumFitness + lastGenerationDiff);
				ImPlot::SetupFinish();

				ImPlot::PlotShadedG("All generations fitness interval", fitnessGetterLambda, &minimumOfGenerations, fitnessGetterLambda, &maximumOfGenerations, numberOfGenerations);
				ImPlot::PlotLine("All generations mean fitness", &meanOfGenerations[0], numberOfGenerations, 1.0, 0.0, defaultPlotLineFlags);

				ImPlot::EndPlot();
			}
		}
	}
	ImGui::End();
}
