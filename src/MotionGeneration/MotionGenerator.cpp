#include "MotionGeneration/MotionGenerator.h"

#include "Logging/SpdlogCommon.h"
#include "Wavelet/HaarWavelet.h"

#include <DTimer/DTimer.h>

#include <algorithm>
#include <functional>

MotionGenerator::MotionGenerator(std::string folder, MotionParameters mP) : motionParameters(mP), database(folder, motionParameters) {
	maxGenerations = 0;
	ea.genesisFunction = std::bind_front(&MotionGenerator::genesisBoundary, this);
	//ea.genesisFunction = std::bind_front(&MotionGenerator::genesisBoundaryWavelet, this);
	//ea.setGenesisFunction(std::bind_front(&MotionGenerator::genesisRandom, this, 256));
	ea.transformFunction = std::bind_front(&MotionGenerator::transform, this);
	ea.evaluationFunction = std::bind_front(&MotionGenerator::evaluate, this);
	ea.fitnessComparisonFunction = [](Spec::Fitness lhs, Spec::Fitness rhs){ return lhs > rhs; };
	ea.variationFunctors = createVariationFunctors();
	ea.survivorSelectionFunction = DEvA::StandardSurvivorSelectors<Spec>::clamp<16>;
	ea.convergenceCheckFunction = std::bind_front(&MotionGenerator::convergenceCheck, this);

	ea.onEpochStartCallback = std::bind_front(&MotionGenerator::onEpochStart, this);
	ea.onEpochEndCallback = std::bind_front(&MotionGenerator::onEpochEnd, this);
	ea.lambda = 10;
	ea.logger.callback = DEvALoggerCallback;
	exportGenerationData();
};

DEvA::StepResult MotionGenerator::search(std::size_t n) {
	maxGenerations = n;
	return ea.search(n);
}

void MotionGenerator::tryExecute_OnMotionGenerationStateChange() {
	if (onMotionGenerationStateChange) {
		onMotionGenerationStateChange(currentGeneration, motionGenerationState);
	}
}

void MotionGenerator::updateMotionGenerationStateWith(EpochProgress & eP) {
	motionGenerationState.updateWith(eP);
	tryExecute_OnMotionGenerationStateChange();
}

void MotionGenerator::updateMotionGenerationStateWithEAProgress() {
	if (ea.genealogy.empty()) [[unlikely]] {
		return;
	}
	EAProgress eP;
	eP.currentGeneration = currentGeneration;
	eP.numberOfGenerations = ea.genealogy.size();
	motionGenerationState.updateWith(eP);
	tryExecute_OnMotionGenerationStateChange();
}

void MotionGenerator::updateMotionGenerationStateWithFitnessStatus() {
	if (ea.genealogy.empty()) [[unlikely]] {
		return;
	}
	auto const & generation = ea.genealogy.back();
	FitnessStatus fS;
	fS.fitnesses.clear();
	for (auto & iptr : generation) {
		if (!iptr->maybePhenotypeProxy.has_value()) {
			continue;
		}
		fS.fitnesses.push_back(iptr->fitness);
	}
	motionGenerationState.updateWith(fS);
	tryExecute_OnMotionGenerationStateChange();
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
