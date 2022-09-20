#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"
#include "EvolutionaryAlgorithm.h"

#include "mgealite_version.h"
#include "Database.h"
#include "MotionGeneration/MotionGenerator.h"

int main() {
	spdlog::set_pattern("[%Y%m%d:%H:%M:%S.%f][%t][%l] %v");
	spdlog::info("MGEALite version: {}", getMGEALiteVersion());

	MotionParameters motionParameters;
	motionParameters.simStart = 0.0;
	motionParameters.simStep = 0.001;
	motionParameters.simSamples = 1024;
	motionParameters.jointNames.push_back("wrist");
	motionParameters.jointNames.push_back("shoulder");
	motionParameters.jointNames.push_back("hip");
	motionParameters.jointLimits.emplace(std::make_pair("wrist", std::make_pair(-50.0, 50.0)));
	motionParameters.jointLimits.emplace(std::make_pair("shoulder", std::make_pair(-250.0, 250.0)));
	motionParameters.jointLimits.emplace(std::make_pair("hip", std::make_pair(-500.0, 500.0)));

	MotionGenerator motionGenerator("./data", motionParameters);
	auto result = motionGenerator.search(100);
	if (DEvA::StepResult::Convergence == result) {
		spdlog::info("Search converged.");
	} else if (DEvA::StepResult::Inconclusive == result) {
		spdlog::info("Search inconclusive.");
	} else if (DEvA::StepResult::StepCount == result) {
		spdlog::info("Step limit reached.");
	}

	return 0;
}
