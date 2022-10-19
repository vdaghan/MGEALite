#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include "EvolutionaryAlgorithm.h"
#include <DTimer/DTimer.h>

#include "mgealite_version.h"
#include "deva_version.h"
#include "Database.h"
#include "GUI/GUIStateDrawer.h"
#include "GUI/GUI.h"
#include "Logging/SpdlogCommon.h"
#include "MotionGeneration/MotionGenerator.h"
#include "SharedSynchronisation.h"

int main() {
	// Logger must be initialised before GUI since GUI needs
	// the MGEALogger to be available to hook into.
	initialiseLogger();
	SharedSynchronisation sharedSync;
	sharedSync.registerEvent("start", false);
	sharedSync.registerEvent("pause", false);
	sharedSync.registerEvent("stop", false);
	sharedSync.registerEvent("exit", false);
	GUI gui(sharedSync.createToken());
	gui.startLoop();
	sharedSync.finaliseAll();

	spdlog::info("DEvA version: {}", getDEvAVersion());
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
	//motionParameters.jointNames.push_back("ankle");
	motionParameters.jointLimits.emplace(std::make_pair("wrist", std::make_pair(-30.0, 30.0)));
	motionParameters.jointLimits.emplace(std::make_pair("shoulder", std::make_pair(-200.0, 200.0)));
	motionParameters.jointLimits.emplace(std::make_pair("hip", std::make_pair(-500.0, 320.0))); // https://bmcsportsscimedrehabil.biomedcentral.com/articles/10.1186/s13102-022-00401-9/figures/1
	//motionParameters.jointLimits.emplace(std::make_pair("ankle", std::make_pair(-70.0, 200.0))); // https://bmcsportsscimedrehabil.biomedcentral.com/articles/10.1186/s13102-022-00401-9/figures/1
	motionParameters.contactParameters = bodyGroundContactParameters();

	MotionGenerator motionGenerator("./data", motionParameters);
	sharedSync.addCallback("stop", CallbackType::OnTrue, [&]() {motionGenerator.stop(); });
	motionGenerator.hookCallbacks<PlotData>(gui.guiStateDrawer.plotData);
	//motionGenerator.onMotionGenerationStateChange = [&](std::size_t gen, MotionGenerationState const & mGS){
	//	gui.state.updateMotionGenerationState(gen, mGS);
	//};

	while (!sharedSync.get("start") and !sharedSync.get("stop")) {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
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
