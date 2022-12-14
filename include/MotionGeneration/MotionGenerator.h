#pragma once

#include "Datastore.h"
#include "MotionGeneration/Initialisers/Initialisers.h"
#include "MotionGeneration/Variations/Variations.h"
#include "MotionGeneration/MotionParameters.h"
#include "MotionGeneration/Specification.h"
#include "SimulationData.h"

#include <string>

class MotionGenerator {
	public:
		MotionGenerator(std::string, MotionParameters);
		DEvA::StepResult search(std::size_t);
		void pause();
		void stop();
		template<typename T> void hookCallbacks(T &);
	private:
		MotionParameters motionParameters;
		Datastore datastore;
		DEvA::EvolutionaryAlgorithm<Spec> ea;
		std::size_t currentGeneration;
		std::size_t maxGenerations;
		std::atomic<bool> pauseFlag;
		bool checkStopFlagAndMaybeWait();
		std::atomic<bool> stopFlag;
		void exportGenerationData();

		// EA Functions
		void createMetricFunctors();
		void createVariationFunctors();
		Spec::MaybePhenotype transform(Spec::Genotype);

		void applyMotionParameters(SimulationDataPtr);
		[[nodiscard]] Spec::Genotypes computeVariation(std::function<SimulationDataPtrs(MGEA::VariationParams, Spec::IndividualPtrs)> vFunc, Spec::IndividualPtrs parents);
		[[nodiscard]] Spec::Genotypes computeGenesis(std::function<SimulationDataPtrs(MGEA::InitialiserParams)> gFunc);

		void onEpochStart(std::size_t);
		void onEpochEnd(std::size_t);
};

template<typename T>
void MotionGenerator::hookCallbacks(T & t) {
	ea.onEAStatsUpdateCallback = std::bind_front(&T::updateEAStatistics, &t);
}
