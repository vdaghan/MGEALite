#pragma once

#include "Database.h"
#include "MotionGeneration/Initialisers/Initialisers.h"
#include "MotionGeneration/Variations/Variations.h"
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
		void pause();
		void stop();
		template<typename T> void hookCallbacks(T &);
		std::function<void(std::size_t, MotionGenerationState const &)> onMotionGenerationStateChange;
	private:
		MotionParameters motionParameters;
		Database database;
		DEvA::EvolutionaryAlgorithm<Spec> ea;
		std::size_t currentGeneration;
		std::size_t maxGenerations;
		EpochProgress epochProgress;
		MotionGenerationState motionGenerationState;
		std::atomic<bool> pauseFlag;
		bool checkStopFlagAndMaybeWait();
		std::atomic<bool> stopFlag;
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
		Spec::MaybePhenotypeProxy transform(Spec::GenotypeProxy);
		Spec::Fitness evaluate(Spec::GenotypeProxy);
		void survivorSelection(std::size_t, Spec::IndividualPtrs &);
		Spec::Distance calculateTorqueDistance(DEvA::IndividualIdentifier, DEvA::IndividualIdentifier);
		Spec::Distance calculateTorqueSignDistance(DEvA::IndividualIdentifier, DEvA::IndividualIdentifier);
		Spec::Distance calculateAngleDistance(DEvA::IndividualIdentifier, DEvA::IndividualIdentifier);
		[[nodiscard]] bool convergenceCheck(Spec::Fitness);

		void applyMotionParameters(SimulationDataPtr);
		[[nodiscard]] Spec::GenotypeProxies computeVariationWithGenotypeProxies(std::function<SimulationDataPtrs(MGEA::VariationParams, SimulationDataPtrs)> vFunc, Spec::GenotypeProxies parentProxies);
		[[nodiscard]] Spec::GenotypeProxies computeVariationWithIndividualPointers(std::function<SimulationDataPtrs(MGEA::VariationParams, Spec::IndividualPtrs)> vFunc, Spec::IndividualPtrs parents);
		[[nodiscard]] Spec::GenotypeProxies computeGenesis(std::function<SimulationDataPtrs(MGEA::InitialiserParams)> gFunc);

		void onEpochStart(std::size_t);
		void onEpochEnd(std::size_t);
};

template<typename T>
void MotionGenerator::hookCallbacks(T & t) {
	ea.onEAStatsUpdateCallback = std::bind_front(&T::updateEAStatistics, &t);
}
