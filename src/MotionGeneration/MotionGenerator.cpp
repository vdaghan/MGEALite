// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/MotionGenerator.h"

#include "MotionGeneration/Initialisers/Initialisers.h"
#include "MotionGeneration/Metrics/Metrics.h"
#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"
#include "Logging/SpdlogCommon.h"
#include "Wavelet/HaarWavelet.h"

#include "DEvA/Project/Filestore.h"
#include <DTimer/DTimer.h>

#include <algorithm>
#include <functional>

MotionGenerator::MotionGenerator(std::string folder, MotionParameters mP) 
: motionParameters(mP)
, datastore(folder)
, pauseFlag(false)
, stopFlag(false)
{
	ea.datastore = std::make_shared<DEvA::Filestore<Spec>>();

	ea.functions.genesisWrapper = [&](auto genesisFunction) {
		typename Spec::Genotypes genotypes(genesisFunction());
		for (auto & genotype : genotypes) {
			applyMotionParameters(genotype);
		}
		return genotypes;
	};
	auto variationWrapper = [&](auto variationFunctor, auto parents) {
		auto children = variationFunctor(parents);
		for (auto& child : children) {
			applyMotionParameters(child);
		}
		return children;
	};
	ea.functions.variationFromGenotypesWrapper = variationWrapper;
	ea.functions.variationFromIndividualPtrsWrapper = variationWrapper;

	setupStandardFunctions(mP);
	ea.importSetup("./EASetup.json");

	maxGenerations = 0;
	std::vector<std::string> paretoMetrics{ "fitness", "balance" };
	Spec::FPSurvivorSelection combinedSurvivorSelectorLambda = [=](DEvA::ParameterMap parameters, Spec::IndividualPtrs & iptrs) {
		MGEA::cullEquals({}, iptrs);
		//MGEA::onlyPositivesIfThereIsAny<double>("fitness", iptrs);
		DEvA::ParameterMap angularVelocitySignMetric({ {"metric", "angularVelocitySign"} });
		JSON j(paretoMetrics);
		DEvA::ParameterMap paretoMetrics({ {"metrics", j}});
		MGEA::survivorSelectionOverMetric(angularVelocitySignMetric, std::bind_front(&MGEA::cullPartiallyDominated, paretoMetrics), iptrs);
		//MGEA::paretoFront(paretoMetrics, iptrs);
	};
	ea.functions.survivorSelection.defineParametrised("EightQueenVariation", combinedSurvivorSelectorLambda, {});
	ea.functions.survivorSelection.use({ "EightQueenVariation" });
	ea.functions.transform.define("Simulate", std::bind_front(&MotionGenerator::transform, this));
	ea.functions.transform.use({ "Simulate" });
	//ea.functions.convergenceCheck.define("StdConvergenceCheckerNever", [](auto T) { return false; });
	//ea.functions.convergenceCheck.use({ "StdConvergenceCheckerNever" });
	
	createMetricFunctors();
	auto compileResult = ea.compile();
	if (not compileResult) {
		spdlog::error("MotionGenerator::MotionGenerator: metrics compile error");
	}
	//ea.useMetricFunctor("angularVelocitySign");
	//ea.useMetricFunctor("balance");
	//ea.useMetricFunctor("fitness");
	auto evaluationCallback = [&](DEvA::IndividualIdentifier id) {
		//auto const & simLogPtr = database.getSimulationLog(id);
		//database.saveSimulationMetrics(simLogPtr->info(), {});
	};
	ea.registerCallback(DEvA::Callback::Evaluation, evaluationCallback);

	ea.onEpochStartCallback = std::bind_front(&MotionGenerator::onEpochStart, this);
	ea.onEpochEndCallback = std::bind_front(&MotionGenerator::onEpochEnd, this);
	ea.onPauseCallback = [&]() { pauseFlag.store(true); };
	ea.onStopCallback = [&]() { stopFlag.store(true); };
	ea.lambda = 256;
	ea.logger.callback = DEvALoggerCallback;
};

DEvA::StepResult MotionGenerator::search(std::size_t n) {
	maxGenerations = n;
	return ea.search(n);
}

void MotionGenerator::pause() {
	ea.pause();
}

void MotionGenerator::stop() {
	ea.stop();
}

bool MotionGenerator::checkStopFlagAndMaybeWait() {
	while (pauseFlag.load() and !stopFlag.load()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return stopFlag.load();
}

