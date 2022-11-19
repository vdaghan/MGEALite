#include "MotionGeneration/MotionGenerator.h"

#include "MotionGeneration/Initialisers/Initialisers.h"
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
	ea.registerEAFunction(DEvA::EAFunction::EvaluateIndividualFromGenotypeProxy, std::bind_front(&MotionGenerator::evaluateIndividualFromGenotypeProxy, this));
	ea.registerEAFunction(DEvA::EAFunction::EvaluateIndividualFromIndividualPtr, std::bind_front(&MotionGenerator::evaluateIndividualFromIndividualPtr, this));
	ea.registerEAFunction(DEvA::EAFunction::SortIndividuals, [&](Spec::IndividualPtr lhs, Spec::IndividualPtr rhs) { return std::get<double>(lhs->metrics.at("fitness")) > std::get<double>(rhs->metrics.at("fitness")); });
	//ea.distanceCalculationFunction = std::bind_front(&MotionGenerator::calculateAngleDistance, this);
	Spec::FMetricComparison balanceComparisonLambda = [](MGEAMetricVariant lhs, MGEAMetricVariant rhs) {
		double lhsBalance(std::get<double>(lhs));
		double rhsBalance(std::get<double>(rhs));
		return lhsBalance < rhsBalance;
	};
	Spec::FMetricComparison fitnessComparisonLambda = [](MGEAMetricVariant lhs, MGEAMetricVariant rhs) {
		double lhsFitness(std::get<double>(lhs));
		double rhsFitness(std::get<double>(rhs));
		return lhsFitness > rhsFitness;
	};
	Spec::FMetricComparison gainComparisonLambda = [](MGEAMetricVariant lhs, MGEAMetricVariant rhs) {
		double lhsGain(std::get<double>(lhs));
		double rhsGain(std::get<double>(rhs));
		return lhsGain > rhsGain;
	};
	Spec::MetricComparisonMap metricComparisonMap{};
	metricComparisonMap.emplace(std::make_pair("balance", balanceComparisonLambda));
	metricComparisonMap.emplace(std::make_pair("fitness", fitnessComparisonLambda));
	metricComparisonMap.emplace(std::make_pair("gain", gainComparisonLambda));
	std::vector<std::string> paretoMetrics{ "fitness", "balance" };
	Spec::FSurvivorSelection combinedSurvivorSelectorLambda = [=](Spec::IndividualPtrs & iptrs) {
		MGEA::cullEquals(iptrs);
		MGEA::onlyPositivesIfThereIsAny("fitness", iptrs);
		MGEA::paretoFront(paretoMetrics, iptrs);
		MGEA::survivorSelectionOverMetric("angularVelocitySign", std::bind_front(&MGEA::cullPartiallyDominated, paretoMetrics, metricComparisonMap), iptrs);
	};
	ea.registerEAFunction(DEvA::EAFunction::SurvivorSelection, combinedSurvivorSelectorLambda);
	ea.registerEAFunction(DEvA::EAFunction::ConvergenceCheck, std::bind_front(&MotionGenerator::convergenceCheck, this));
	ea.registerMetricComparison("balance", balanceComparisonLambda);
	ea.registerMetricComparison("fitness", fitnessComparisonLambda);
	ea.registerMetricComparison("gain", gainComparisonLambda);

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
	ea.lambda = 64;
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
