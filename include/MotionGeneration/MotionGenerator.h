#pragma once

#include "Datastore.h"
#include "MotionGeneration/Initialisers/Initialisers.h"
#include "MotionGeneration/Variations/Variations.h"
#include "MotionGeneration/MotionParameters.h"
#include "MotionGeneration/Specification.h"
#include "SimulationData.h"

#include <string>

class MotionGenerator : public DEvA::EvolutionaryAlgorithm<Spec> {
	public:
		MotionGenerator(std::string, std::string);
		DEvA::StepResult search(std::size_t);
		template<typename T> void hookCallbacks(T &);

		MotionParameters motionParameters;
	private:
		void importMotionParameters(std::filesystem::path const &);
		Datastore datastore;
		void setupStandardFunctions();

		// EA Functions
		Spec::MaybePhenotype transform(Spec::Genotype);

		void applyMotionParameters(SimulationDataPtr);

		void onEpochStart(std::size_t);
		void onEpochEnd(std::size_t);
};

template<typename T>
void MotionGenerator::hookCallbacks(T & t) {
	onEAStatsUpdateCallback = std::bind_front(&T::updateEAStatistics, &t);
}
