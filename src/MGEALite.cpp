#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "EvolutionaryAlgorithm.h"
#include <DTimer/DTimer.h>

#include "mgealite_version.h"
#include "Database.h"
#include "GUI/GUIState.h"
#include "GUI/GUIStateDrawer.h"
#include "GUI/GUI.h"
#include "Logging/SpdlogCommon.h"
#include "MotionGeneration/MotionGenerator.h"

int main() {
	GUI<GUIState> gui;
	gui.startLoop();

	initialiseLogger();
	spdlog::info("MGEALite version: {}", getMGEALiteVersion());

	MotionParameters motionParameters;
	motionParameters.simStart = 0.0;
	motionParameters.simStep = 0.01;
	motionParameters.simSamples = 256;
	motionParameters.alignment = -1;
	motionParameters.timeout = 300.0;
	motionParameters.jointNames.push_back("wrist");
	motionParameters.jointNames.push_back("shoulder");
	motionParameters.jointNames.push_back("hip");
	motionParameters.jointNames.push_back("ankle");
	motionParameters.jointLimits.emplace(std::make_pair("wrist", std::make_pair(-30.0, 30.0)));
	motionParameters.jointLimits.emplace(std::make_pair("shoulder", std::make_pair(-200.0, 200.0)));
	motionParameters.jointLimits.emplace(std::make_pair("hip", std::make_pair(-320.0, 500.0))); // https://bmcsportsscimedrehabil.biomedcentral.com/articles/10.1186/s13102-022-00401-9/figures/1
	motionParameters.jointLimits.emplace(std::make_pair("ankle", std::make_pair(-70.0, 200.0))); // https://bmcsportsscimedrehabil.biomedcentral.com/articles/10.1186/s13102-022-00401-9/figures/1
	motionParameters.contactParameters = bodyGroundContactParameters();

	MotionGenerator motionGenerator("./data", motionParameters);
	motionGenerator.onEpochEndCallback = [&](Spec::Genealogy const & g){ gui.state.updateGenealogy(g); };
	auto result = motionGenerator.search(250);
	if (DEvA::StepResult::Convergence == result) {
		spdlog::info("Search converged.");
	} else if (DEvA::StepResult::Inconclusive == result) {
		spdlog::info("Search inconclusive.");
	} else if (DEvA::StepResult::StepCount == result) {
		spdlog::info("Step limit reached.");
	}

	spdlog::info("\n{}", DTimer::print());
	gui.stopLoop();
	return 0;
}
