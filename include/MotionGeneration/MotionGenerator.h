#pragma once

#include "Database.h"
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
		MotionGenerator(std::string);
		void search(std::size_t);
	private:
		Database database;
		DEvA::EvolutionaryAlgorithm<Spec> ea;
		std::size_t currentGeneration;
		void exportGenerationData();

		Spec::Generation genesis();
		Spec::GenotypeProxy createGenotype();
		Spec::PhenotypeProxy transform(Spec::GenotypeProxy);
		Spec::Fitness evaluate(Spec::GenotypeProxy);
		Spec::IndividualPtrs parentSelection(Spec::IndividualPtrs);
		void survivorSelection(Spec::IndividualPtrs &);
		bool convergenceCheck(Spec::Fitness);

		Spec::GenotypeProxies cutAndCrossfillVariation(Spec::GenotypeProxies);

		void onEpochStart(std::size_t);
		void onEpochEnd(std::size_t);
};
