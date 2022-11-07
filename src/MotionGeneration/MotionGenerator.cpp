#include "MotionGeneration/MotionGenerator.h"

#include "MotionGeneration/Initialisers/Initialisers.h"
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
	//ea.genesisFunction = [&]() { return computeGenesis(std::bind_front(MGEA::genesisRandom, 128)); };
	//ea.genesisFunction = [&]() { return computeGenesis(MGEA::genesisBoundary); };
	//ea.genesisFunction = [&]() { return computeGenesis(MGEA::genesisBoundaryWavelet); };
	ea.registerEAFunction(DEvA::EAFunction::Initialisation, [&]() { return computeGenesis(std::bind_front(MGEA::genesisZero)); });
	ea.registerEAFunction(DEvA::EAFunction::Transformation, std::bind_front(&MotionGenerator::transform, this));
	ea.registerEAFunction(DEvA::EAFunction::EvaluateIndividualFromGenotypeProxy, std::bind_front(&MotionGenerator::evaluateIndividualFromGenotypeProxy, this));
	ea.registerEAFunction(DEvA::EAFunction::EvaluateIndividualFromIndividualPtr, std::bind_front(&MotionGenerator::evaluateIndividualFromIndividualPtr, this));
	ea.registerEAFunction(DEvA::EAFunction::SortIndividuals, [&](Spec::IndividualPtr lhs, Spec::IndividualPtr rhs) { return std::get<double>(lhs->metrics.at("fitness")) > std::get<double>(rhs->metrics.at("fitness")); });
	//ea.distanceCalculationFunction = std::bind_front(&MotionGenerator::calculateAngleDistance, this);
	//ea.survivorSelectionFunction = DEvA::StandardSurvivorSelectors<Spec>::clamp<128>;
	//ea.survivorSelectionFunction = std::bind_front(&MotionGenerator::survivorSelection, this, 32);
	ea.registerEAFunction(DEvA::EAFunction::SurvivorSelection, std::bind_front(&MotionGenerator::survivorSelectionPareto, this));
	ea.registerEAFunction(DEvA::EAFunction::ConvergenceCheck, std::bind_front(&MotionGenerator::convergenceCheck, this));
	ea.registerMetricComparison("fitness", [](MGEAMetricVariant lhs, MGEAMetricVariant rhs){
		double lhsFitness(std::get<double>(lhs));
		double rhsFitness(std::get<double>(rhs));
		return lhsFitness > rhsFitness;
	});
	ea.registerMetricComparison("gain", [](MGEAMetricVariant lhs, MGEAMetricVariant rhs) {
		double lhsGain(std::get<double>(lhs));
		double rhsGain(std::get<double>(rhs));
		return lhsGain > rhsGain;
	});

	createVariationFunctors();
	ea.useVariationFunctor("CrossoverAll");
	ea.useVariationFunctor("DeletionLInt");
	ea.useVariationFunctor("DirectionalLInt");
	ea.useVariationFunctor("SNVLInt");

	ea.onEpochStartCallback = std::bind_front(&MotionGenerator::onEpochStart, this);
	ea.onEpochEndCallback = std::bind_front(&MotionGenerator::onEpochEnd, this);
	ea.onPauseCallback = [&]() { pauseFlag.store(true); };
	ea.onStopCallback = [&]() { stopFlag.store(true); };
	ea.lambda = 256;
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
