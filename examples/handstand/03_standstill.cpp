// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <chrono>
#include <iostream>
#include <fstream>
#include <numbers>
#include <string>
#include <thread>
#include <vector>

#include "MGEA.h"

bool fitnessBetterThan(std::any const & lhs, std::any const & rhs) {
	auto lhsPair(std::any_cast<MGEA::SteppedDouble>(lhs));
	auto rhsPair(std::any_cast<MGEA::SteppedDouble>(rhs));
	if (lhsPair.first != rhsPair.first) {
		return lhsPair.first > rhsPair.first;
	}
	return lhsPair.second < rhsPair.second;
}

int main() {
	spdlog::info("DEvA version: {}", getDEvAVersion());
	spdlog::info("MGEALite version: {}", getMGEALiteVersion());

	MotionGenerator motionGenerator("./data", "./03_standstill.json");
	motionGenerator.metricFunctors.orderings.emplace(std::pair("fitnessBetterThan", fitnessBetterThan));
	motionGenerator.metricFunctors.equivalences.emplace(std::pair("fitnessEquivalence", MGEA::steppedDoubleEquivalence));
	motionGenerator.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("computeFitness", MGEA::maximumAngleDifferenceStepped));
	motionGenerator.metricFunctors.metricToJSONObjectFunctions.emplace(std::pair("fitnessToJSON", MGEA::steppedDoubleConversion));

	auto compileResult = motionGenerator.compile();
	motionGenerator.lambda = 256;
	auto result = motionGenerator.search(50);

	return 0;
}
