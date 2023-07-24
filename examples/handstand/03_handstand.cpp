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

	MotionGenerator motionGenerator("./data", "./03_handstand.json");

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
		auto & fingertipX = simDataPtr->outputs.at("fingertipX");
		auto & fingertipZ = simDataPtr->outputs.at("fingertipZ");
		double fingertipZSum = std::accumulate(fingertipZ.begin(), fingertipZ.end(), 0.0);
		double maxFingertipZ = *std::max_element(fingertipZ.begin(), fingertipZ.end());
		auto & palmX = simDataPtr->outputs.at("palmX");
		auto & palmZ = simDataPtr->outputs.at("palmZ");
		double palmZSum = std::accumulate(palmZ.begin(), palmZ.end(), 0.0);
		double maxPalmZ = *std::max_element(palmZ.begin(), palmZ.end());
		auto & comZ = simDataPtr->outputs.at("centerOfMassZ");
		double comZSum = std::accumulate(comZ.begin(), comZ.end(), 0.0, absLambda);
		double maxComZ = *std::max_element(comZ.begin(), comZ.end());

		auto fingertipXFirst(fingertipX.front());
		auto fingertipZFirst(fingertipZ.front());
		auto palmXFirst(palmX.front());
		auto palmZFirst(palmZ.front());

		double maxFingertipDisplacement(0.0);
		double fingertipDisplacement(0.0);
		for (std::size_t i(0); i < fingertipX.size(); ++i) {
			auto displacement(std::sqrt(std::pow(std::abs(fingertipXFirst - fingertipX[i]), 2) + std::pow(std::abs(fingertipZFirst - fingertipZ[i]), 2)));
			fingertipDisplacement += displacement;
			maxFingertipDisplacement = std::max(maxFingertipDisplacement, displacement);
		}
		fingertipDisplacement /= static_cast<double>(fingertipX.size());
		double maxPalmDisplacement(0.0);
		double palmDisplacement(0.0);
		for (std::size_t i(0); i < palmX.size(); ++i) {
			auto displacement(std::sqrt(std::pow(std::abs(palmXFirst - palmX[i]), 2) + std::pow(std::abs(palmZFirst - palmZ[i]), 2)));
			palmDisplacement += displacement;
			maxPalmDisplacement = std::max(maxPalmDisplacement, displacement);
		}
		palmDisplacement /= static_cast<double>(palmX.size());

		double totalDisplacement(fingertipDisplacement + palmDisplacement);

		double threshold(0.01);
		double fitness;
		if (totalDisplacement <= threshold and maxFingertipDisplacement <= threshold and maxPalmDisplacement <= threshold) {
			fitness = comZSum * timeStep / motionGenerator.motionParameters.simStop();
		} else {
			fitness = -totalDisplacement - 0.5*(maxFingertipDisplacement + maxPalmDisplacement);
		}

		return fitness;
	};

	motionGenerator.functions.survivorSelection.defineParametrised("Combined", combinedSurvivorSelectorLambda, {});
	motionGenerator.functions.survivorSelection.compile("CombinedCompiled", { "Combined" });
	motionGenerator.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("computeFitness", computeFitnessLambda));

	auto compileResult = motionGenerator.compile();
	motionGenerator.lambda = 1024;
	auto result = motionGenerator.search(1024);

	return 0;
}
