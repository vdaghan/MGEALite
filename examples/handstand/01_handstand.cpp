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

int main() {
	spdlog::info("DEvA version: {}", getDEvAVersion());
	spdlog::info("MGEALite version: {}", getMGEALiteVersion());

	MotionGenerator motionGenerator("./data", "./01_handstand.json");

	std::vector<std::string> paretoMetrics{ "fitness", "balance" };
	Spec::BPSurvivorSelection::Parametrisable combinedSurvivorSelectorLambda = [=](DEvA::ParameterMap parameters, Spec::IndividualPtrs iptrs) {
		iptrs = MGEA::cullEquals({}, iptrs);
		iptrs = MGEA::onlyPositivesIfThereIsAny<double>("fitness", iptrs);
		JSON j(paretoMetrics);
		iptrs = MGEA::paretoFront(DEvA::ParameterMap({ {"metrics", j} }), iptrs);
		return iptrs;
	};
	auto computeFitnessLambda = [&](DEvA::ParameterMap pMap, Spec::IndividualPtr iptr) {
		auto & simDataPtr(*iptr->maybePhenotype);
		double timeStep = motionGenerator.motionParameters.simStep;
		auto absLambda = [](double prev, double next) {
			return prev + std::abs(next);
		};
		auto & fingertipZ = simDataPtr->outputs.at("fingertipZ");
		double fingertipZSum = std::accumulate(fingertipZ.begin(), fingertipZ.end(), 0.0);
		double maxFingertipZ = *std::max_element(fingertipZ.begin(), fingertipZ.end());
		auto & palmZ = simDataPtr->outputs.at("palmZ");
		double palmZSum = std::accumulate(palmZ.begin(), palmZ.end(), 0.0);
		double maxPalmZ = *std::max_element(palmZ.begin(), palmZ.end());
		auto & comZ = simDataPtr->outputs.at("centerOfMassZ");
		double comZSum = std::accumulate(comZ.begin(), comZ.end(), 0.0, absLambda);
		double maxComZ = *std::max_element(comZ.begin(), comZ.end());
		auto & toeZ = simDataPtr->outputs.at("toeZ");
		double toeZSum = std::accumulate(toeZ.begin(), toeZ.end(), 0.0);
		auto & ankleZ = simDataPtr->outputs.at("ankleZ");
		double ankleZSum = std::accumulate(ankleZ.begin(), ankleZ.end(), 0.0);

		double fitness;
		//if (fingertipZSum <= 0.0 and palmZSum <= 0.0) {
		if (maxFingertipZ <= 0.001 and maxPalmZ <= 0.001) {
			fitness = comZSum * timeStep / motionGenerator.motionParameters.simStop();
			//fitness = maxComZ;
			//fitness = toeZSum * timeStep / motionGenerator.motionParameters.simStop();
			//fitness = ankleZSum * timeStep / motionGenerator.motionParameters.simStop();
		} else {
			fitness = 0.0;
			if (maxFingertipZ > 0.001) {
				fitness -= maxFingertipZ;
			}
			if (maxPalmZ > 0.001) {
				fitness -= maxPalmZ;
			}
		}

		return fitness;
	};

	motionGenerator.functions.survivorSelection.defineParametrised("Combined", combinedSurvivorSelectorLambda, {});
	motionGenerator.functions.survivorSelection.compile("CombinedCompiled", { "Combined" });
	motionGenerator.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("computeFitness", computeFitnessLambda));

	auto compileResult = motionGenerator.compile();
	motionGenerator.lambda = 256;
	auto result = motionGenerator.search(1024);

	return 0;
}
