#pragma once

#include "Database.h"
#include "MotionGeneration/MotionGenerationState.h"
#include "MotionGeneration/MotionParameters.h"
#include "MotionGeneration/Specification.h"
#include "SimulationData.h"

#include "EvolutionaryAlgorithm.h"

#include <string>

class MotionGenerator {
	public:
		MotionGenerator(std::string, MotionParameters);
		DEvA::StepResult search(std::size_t);
		std::function<void(std::size_t, MotionGenerationState const &)> onMotionGenerationStateChange;
	private:
		MotionParameters motionParameters;
		Database database;
		DEvA::EvolutionaryAlgorithm<Spec> ea;
		std::size_t currentGeneration;
		std::size_t maxGenerations;
		EpochProgress epochProgress;
		MotionGenerationState motionGenerationState;
		void tryExecute_OnMotionGenerationStateChange();
		void updateMotionGenerationStateWith(EpochProgress &);
		void updateMotionGenerationStateWithEAProgress();
		void updateMotionGenerationStateWithFitnessStatus();
		void exportGenerationData();

		// EA Functions
		std::list<Spec::SVariationFunctor> createVariationFunctors();
		Spec::GenotypeProxies genesisBoundary();
		Spec::GenotypeProxies genesisBoundaryWavelet();
		Spec::GenotypeProxies genesisRandom(std::size_t);
		Spec::GenotypeProxy createGenotype();
		Spec::MaybePhenotypeProxy transform(Spec::GenotypeProxy);
		Spec::Fitness evaluate(Spec::GenotypeProxy);
		template <std::size_t, std::size_t>
		Spec::IndividualPtrs parentSelection(Spec::IndividualPtrs);
		[[nodiscard]] bool convergenceCheck(Spec::Fitness);

		using VariationInterface = std::function<SimulationDataPtrs(MotionParameters const &, SimulationDataPtrs)>;
		[[nodiscard]] Spec::GenotypeProxies computeVariation(VariationInterface, Spec::GenotypeProxies);

		void onEpochStart(std::size_t);
		void onEpochEnd(std::size_t);
};
