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

	MotionGenerator motionGenerator("./data");
	auto result = motionGenerator.search(2);
	if (DEvA::StepResult::Convergence == result) {
		spdlog::info("Search converged.");
	} else if (DEvA::StepResult::Inconclusive == result) {
		spdlog::info("Search inconclusive.");
	} else if (DEvA::StepResult::StepCount == result) {
		spdlog::info("Step limit reached.");
	}

	return 0;
}
