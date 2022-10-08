#include "GUI/GUIStateDrawer.h"

#include "GUI/Initialisation.h"

#include "imgui.h"
#include "implot.h"

#include <algorithm>
#include <limits>

void GUIStateDrawer::draw(GUIState & state) {
	drawPlots(state);
}

void GUIStateDrawer::drawPlots(GUIState & state) {
	std::lock_guard<std::mutex> lock(state.getMutexRef());

	ImPlotFlags defaultPlotFlags = ImPlotFlags_NoTitle
		| ImPlotFlags_NoMenus
		| ImPlotFlags_Crosshairs;
	ImPlotAxisFlags defaultAxisFlags = ImPlotAxisFlags_NoGridLines
		| ImPlotAxisFlags_NoMenus
		| ImPlotAxisFlags_NoSideSwitch
		| ImPlotAxisFlags_NoHighlight;
	ImPlotLineFlags defaultPlotLineFlags = ImPlotLineFlags_NoClip;
	//auto & imguiStyle = ImGui::GetStyle();
	//imguiStyle.WindowMinSize = ImVec2(480, 270);
	auto & plotStyle = ImPlot::GetStyle();
	plotStyle.PlotPadding = ImVec2(1, 1);
	plotStyle.LabelPadding = ImVec2(1, 1);
	plotStyle.PlotDefaultSize = ImVec2(480 - 8, 270 - 8);
	plotStyle.PlotMinSize = ImVec2(480 - 8, 270 - 8);

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(2 * 480, 270), ImGuiCond_Always);
	ImGui::Begin("##Plots", NULL, ImGuiWindowFlags_NoTitleBar);
	if (!state.minFitness.empty()) {
		auto & minFitness = state.minFitness;
		auto & maxFitness = state.maxFitness;
		auto & lastFitness = state.lastFitness;

		if (ImPlot::BeginPlot("All Generation Plots", ImVec2(0, 0), defaultPlotFlags)) {
			ImPlot::SetupAxis(ImAxis_X1, "Individual", defaultAxisFlags);
			ImPlot::SetupAxis(ImAxis_Y1, "Fitness", defaultAxisFlags);
			ImPlot::SetupLegend(ImPlotLocation_SouthWest);

			double numIndividuals = double(lastFitness.size());
			auto const & minFitnessIt = std::min_element(lastFitness.begin(), lastFitness.end());
			auto const & maxFitnessIt = std::max_element(lastFitness.begin(), lastFitness.end());
			double minSize = *minFitnessIt;
			double maxSize = *maxFitnessIt;
			double diff = maxSize - minSize;
			double offset = diff * 0.05;
			ImPlot::SetupAxisLimits(ImAxis_X1, 0, numIndividuals);
			ImPlot::SetupAxisLimits(ImAxis_Y1, minSize - offset, maxSize + offset);
			ImPlot::SetupFinish();

			auto minGetterLambda = [](int idx, void * user_data) -> ImPlotPoint {
				std::vector<Spec::Fitness> * minFitness = static_cast<std::vector<Spec::Fitness> *>(user_data);
				return ImPlotPoint(idx, minFitness->at(idx));
			};
			auto maxGetterLambda = [](int idx, void * user_data) -> ImPlotPoint {
				std::vector<Spec::Fitness> * maxFitness = static_cast<std::vector<Spec::Fitness> *>(user_data);
				return ImPlotPoint(idx, maxFitness->at(idx));
			};
			ImPlot::PlotShadedG("All generations fitness interval", minGetterLambda, &minFitness, maxGetterLambda, &maxFitness, minFitness.size());
			ImPlot::PlotLine("Last Generation Fitness", &lastFitness[0], lastFitness.size(), 1.0, 0.0, defaultPlotLineFlags);

			ImPlot::EndPlot();
		}
	}
	ImGui::SameLine();
	if (!state.minFitnesses.empty()) {
		auto & minFitnesses = state.minFitnesses;
		auto & maxFitnesses = state.maxFitnesses;
		auto & meanFitnesses = state.meanFitnesses;
		if (ImPlot::BeginPlot("Fitness Evolution", ImVec2(0, 0), defaultPlotFlags)) {
			ImPlot::SetupAxis(ImAxis_X1, "Generation", defaultAxisFlags);
			ImPlot::SetupAxis(ImAxis_Y1, "Fitness", defaultAxisFlags);
			ImPlot::SetupLegend(ImPlotLocation_SouthEast);

			double numIndividuals = double(minFitnesses.size());
			auto const & minFitnessIt = std::min_element(minFitnesses.begin(), minFitnesses.end());
			auto const & maxFitnessIt = std::max_element(maxFitnesses.begin(), maxFitnesses.end());
			double minSize = *minFitnessIt;
			double maxSize = *maxFitnessIt;
			double diff = maxSize - minSize;
			double offset = diff * 0.05;
			ImPlot::SetupAxisLimits(ImAxis_X1, 0, numIndividuals);
			ImPlot::SetupAxisLimits(ImAxis_Y1, minSize - offset, maxSize + offset);
			ImPlot::SetupFinish();

			auto minGetterLambda = [](int idx, void * user_data) -> ImPlotPoint {
				std::vector<Spec::Fitness> * minFitnesses = static_cast<std::vector<Spec::Fitness> *>(user_data);
				return ImPlotPoint(idx, minFitnesses->at(idx));
			};
			auto maxGetterLambda = [](int idx, void * user_data) -> ImPlotPoint {
				std::vector<Spec::Fitness> * maxFitnesses = static_cast<std::vector<Spec::Fitness> *>(user_data);
				return ImPlotPoint(idx, maxFitnesses->at(idx));
			};
			ImPlot::PlotShadedG("All generations fitness interval", minGetterLambda, &minFitnesses, maxGetterLambda, &maxFitnesses, maxFitnesses.size());
			ImPlot::PlotLine("All generations mean fitness", &meanFitnesses[0], meanFitnesses.size(), 1.0, 0.0, defaultPlotLineFlags);

			ImPlot::EndPlot();
		}
	}
	ImGui::End();
}
