#pragma once

#include "Database.h"
#include "MotionParameters.h"
#include "SimulationData.h"

#include "EvolutionaryAlgorithm.h"
#include "Specialisation.h"

#include <string>

struct Specification {
	Specification(Database & database_) : database{database_} {};
	Database & database;
	using Genotype = SimulationDataPtr;
	using GenotypeProxy = SimulationInfo;
	using Phenotype = SimulationDataPtr;
	using PhenotypeProxy = SimulationInfo;
	using Fitness = double;
	using IndividualParameters = DEvA::NullVParameters;
};
using Spec = DEvA::Specialisation<Specification>;

class MotionGenerator {
	public:
		MotionGenerator(std::string, MotionParameters);
		DEvA::StepResult search(std::size_t);
	private:
		Database database;
		MotionParameters motionParameters;
		DEvA::EvolutionaryAlgorithm<Spec> ea;
		std::size_t currentGeneration;
		void exportGenerationData();

		Spec::Generation genesis();
		Spec::GenotypeProxy createGenotype();
		Spec::MaybePhenotypeProxy transform(Spec::GenotypeProxy);
		Spec::Fitness evaluate(Spec::GenotypeProxy);
		template <std::size_t, std::size_t>
		Spec::IndividualPtrs parentSelection(Spec::IndividualPtrs);
		void survivorSelection(Spec::IndividualPtrs &);
		[[nodiscard]] bool convergenceCheck(Spec::Fitness);

		[[nodiscard]] Spec::GenotypeProxies computeVariation(std::function<SimulationDataPtrs(SimulationDataPtrs)>, Spec::GenotypeProxies);

		void onEpochStart(std::size_t);
		void onEpochEnd(std::size_t);
};
