#pragma once

#include "Database.h"
#include "SimulationData.h"

#include "EvolutionaryAlgorithm.h"
#include "Specialisation.h"

#include <string>

struct Specification {
	Specification(Database & database_) : database{database_} {};
	Database & database;
	using Genotype = std::vector<size_t>;
	using GenotypeProxy = SimulationInfo;
	using Phenotype = std::vector<size_t>;
	using PhenotypeProxy = SimulationInfo;
	using Fitness = double;
	using IndividualParameters = DEvA::NullVParameters;
};
using Spec = DEvA::Specialisation<Specification>;

class MotionGenerator {
	public:
		MotionGenerator(std::string);
		void epoch();
	private:
		Database database;
		DEvA::EvolutionaryAlgorithm<Spec> ea;
		void exportGenerationData();

		Spec::Generation genesis();
		Spec::GenotypeProxy createGenotype();
		Spec::PhenotypeProxy transform(Spec::GenotypeProxy);
		Spec::Fitness evaluate(Spec::GenotypeProxy);
		Spec::IndividualPtrs parentSelection(Spec::IndividualPtrs);
		void survivorSelection(Spec::IndividualPtrs &);
		bool convergenceCheck(Spec::Fitness);

		Spec::GenotypeProxies cutAndCrossfillVariation(Spec::GenotypeProxies);
};
