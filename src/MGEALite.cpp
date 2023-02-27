// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include "GUI/GUIStateDrawer.h"
#include "GUI/GUI.h"
#include "SharedSynchronisation.h"
#include <DTimer/DTimer.h>

#include "MGEA.h"

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

	MotionGenerator motionGenerator("./data", "./EASetup.json");

	std::vector<std::string> paretoMetrics{ "fitness", "balance" };
	Spec::FPSurvivorSelection combinedSurvivorSelectorLambda = [=](DEvA::ParameterMap parameters, Spec::IndividualPtrs & iptrs) {
		MGEA::cullEquals({}, iptrs);
		//MGEA::onlyPositivesIfThereIsAny<double>("fitness", iptrs);
		DEvA::ParameterMap angularVelocitySignMetric({ {"metric", "angularVelocitySign"} });
		JSON j(paretoMetrics);
		DEvA::ParameterMap paretoMetrics({ {"metrics", j} });
		MGEA::survivorSelectionOverMetric(angularVelocitySignMetric, std::bind_front(&MGEA::cullPartiallyDominated, paretoMetrics), iptrs);
		MGEA::paretoFront(paretoMetrics, iptrs);
	};
	auto computeBalanceLambda = [&](Spec::IndividualPtr iptr) {
		auto & simDataPtr(*iptr->maybePhenotype);

		auto & comX = simDataPtr->outputs.at("centerOfMassX");
		auto & palmX = simDataPtr->outputs.at("palmX");
		double balance = std::inner_product(comX.begin(), comX.end(), palmX.begin(), 0.0, std::plus<>(), [](auto const & cX, auto const & pX) {
			return std::abs(cX - pX);
			});
		double timeStep = motionGenerator.motionParameters.simStep;
		balance = balance * timeStep / motionGenerator.motionParameters.simStop();
		return balance;
	};
	auto computeFitnessLambda = [&](Spec::IndividualPtr iptr) {
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

		double fitness;
		//if (fingertipZSum <= 0.0 and palmZSum <= 0.0) {
		if (maxFingertipZ <= 0.001 and maxPalmZ <= 0.001) {
			fitness = comZSum * timeStep / motionGenerator.motionParameters.simStop();
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

	motionGenerator.functions.survivorSelection.defineParametrised("EightQueenVariation", combinedSurvivorSelectorLambda, {});
	motionGenerator.functions.survivorSelection.use({ "EightQueenVariation" });
	motionGenerator.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("computeBalance", computeBalanceLambda));
	motionGenerator.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("computeFitness", computeFitnessLambda));

	auto compileResult = motionGenerator.compile();
	if (not compileResult) {
		spdlog::error("MotionGenerator::MotionGenerator: metrics compile error");
	}

	motionGenerator.lambda = 256;

	sharedSync.addCallback("stop", CallbackType::OnTrue, [&]() {motionGenerator.stop(); });
	motionGenerator.hookCallbacks<PlotData>(gui.guiStateDrawer.plotData);
	//motionGenerator.onMotionGenerationStateChange = [&](std::size_t gen, MotionGenerationState const & mGS){
	//	gui.state.updateMotionGenerationState(gen, mGS);
	//};

	while (!sharedSync.get("start") and !sharedSync.get("stop")) {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
	auto result = motionGenerator.search(50);
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
