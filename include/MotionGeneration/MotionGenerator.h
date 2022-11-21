#pragma once

#include "Database.h"
#include "MotionGeneration/Initialisers/Initialisers.h"
#include "MotionGeneration/Variations/Variations.h"
#include "MotionGeneration/Metrics/Metrics.h"
#include "MotionGeneration/MotionParameters.h"
#include "MotionGeneration/Specification.h"
#include "SimulationData.h"

#include "DEvA/EvolutionaryAlgorithm.h"

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
		Database database;
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
		Spec::MaybePhenotypeProxy transform(Spec::GenotypeProxy);
		Spec::MetricVariantMap evaluateIndividualFromGenotypeProxy(Spec::GenotypeProxy);
		Spec::MetricVariantMap evaluateIndividualFromIndividualPtr(Spec::IndividualPtr);
		Spec::Distance calculateTorqueDistance(DEvA::IndividualIdentifier, DEvA::IndividualIdentifier);
		Spec::Distance calculateTorqueSignDistance(DEvA::IndividualIdentifier, DEvA::IndividualIdentifier);
		Spec::Distance calculateAngleDistance(DEvA::IndividualIdentifier, DEvA::IndividualIdentifier);
		[[nodiscard]] bool convergenceCheck(Spec::MetricVariantMap);

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
