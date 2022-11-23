// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/MotionGenerator.h"

#include "MotionGeneration/Initialisers/Initialisers.h"
#include "MotionGeneration/Metrics/Metrics.h"
#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"
#include "Logging/SpdlogCommon.h"
#include "Wavelet/HaarWavelet.h"

#include <DTimer/DTimer.h>

#include <algorithm>
#include <functional>

MotionGenerator::MotionGenerator(std::string folder, MotionParameters mP) 
: motionParameters(mP)
, database(folder, motionParameters)
, pauseFlag(false)
, stopFlag(false)
{
	maxGenerations = 0;
	ea.registerEAFunction(DEvA::EAFunction::Initialisation, [&]() { return computeGenesis(std::bind_front(MGEA::genesisZero)); });
	ea.registerEAFunction(DEvA::EAFunction::Transformation, std::bind_front(&MotionGenerator::transform, this));
	ea.registerEAFunction(DEvA::EAFunction::SortIndividuals, [&](Spec::IndividualPtr lhs, Spec::IndividualPtr rhs) {
		return lhs->metricMap.at("fitness") < rhs->metricMap.at("fitness");
	});
	std::vector<std::string> paretoMetrics{ "fitness", "balance" };
	Spec::FSurvivorSelection combinedSurvivorSelectorLambda = [=](Spec::IndividualPtrs & iptrs) {
		MGEA::cullEquals(iptrs);
		MGEA::onlyPositivesIfThereIsAny<double>("fitness", iptrs);
		MGEA::paretoFront(paretoMetrics, iptrs);
		MGEA::survivorSelectionOverMetric("angularVelocitySign", std::bind_front(&MGEA::cullPartiallyDominated, paretoMetrics), iptrs);
	};
	ea.registerEAFunction(DEvA::EAFunction::SurvivorSelection, combinedSurvivorSelectorLambda);
	ea.registerEAFunction(DEvA::EAFunction::ConvergenceCheck, [](Spec::SMetricMap const& metricMap) { return false; });

	createMetricFunctors();
	ea.useMetricFunctor("angularVelocitySign");
	ea.useMetricFunctor("balance");
	ea.useMetricFunctor("fitness");
	auto evaluationCallback = [&](DEvA::IndividualIdentifier id) {
		auto const & simLogPtr = database.getSimulationLog(id);
		database.saveSimulationMetrics(simLogPtr->info(), {});
	};
	ea.registerCallback(DEvA::Callback::Evaluation, evaluationCallback);
	createVariationFunctors();
	ea.useVariationFunctor("CrossoverAll");
	ea.useVariationFunctor("DeletionLIntBP");
	ea.useVariationFunctor("DeletionLIntFP");
	ea.useVariationFunctor("DirectionalLIntBP");
	ea.useVariationFunctor("DirectionalLIntFP");
	ea.useVariationFunctor("SNVLIntBP");
	ea.useVariationFunctor("SNVLIntFP");

	ea.onEpochStartCallback = std::bind_front(&MotionGenerator::onEpochStart, this);
	ea.onEpochEndCallback = std::bind_front(&MotionGenerator::onEpochEnd, this);
	ea.onPauseCallback = [&]() { pauseFlag.store(true); };
	ea.onStopCallback = [&]() { stopFlag.store(true); };
	ea.lambda = 1024;
	ea.logger.callback = DEvALoggerCallback;
	exportGenerationData();
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

void MotionGenerator::exportGenerationData() {
	spdlog::info("Exporting previous data to EA...");
	SimulationHistory const & simulationHistory = database.getSimulationHistory();
	if (simulationHistory.empty()) {
		spdlog::info("No previous data found.");
		return;
	}
	auto const & lastElement = std::max_element(simulationHistory.begin(), simulationHistory.end(), [](auto const & lhs, auto const & rhs){ return lhs.first.identifier < rhs.first.identifier; });
	std::size_t lastGeneration = lastElement->first.generation;
	spdlog::info("Last run had {} generations.", lastGeneration);

	for (std::size_t gen(0); gen <= lastGeneration; ++gen) {
		Spec::Generation generation;
		for (auto const & historyPair : simulationHistory) {
			auto const & simInfo = historyPair.first;
			if (simInfo.generation != gen) {
				continue;
			}
			Spec::IndividualPtr iptr = std::make_shared<Spec::SIndividual>(simInfo.generation, simInfo.identifier, simInfo);
			generation.emplace_back(iptr);
		}
		ea.addGeneration(generation);
	}
}
