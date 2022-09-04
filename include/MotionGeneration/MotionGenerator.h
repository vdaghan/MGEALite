#pragma once

#include "Database.h"
#include "SimulationData.h"

#include "EvolutionaryAlgorithm.h"
#include "Specification.h"
#include "Specialisation.h"

#include <string>


using Spec = DEvA::Specialisation<Specification>;

class MotionGenerator {
	public:
		MotionGenerator(std::string);
		void start();
	private:
		Database database;
		DEvA::EvolutionaryAlgorithm<Spec> ea;
		Spec::Generation genesis();
		Spec::GenotypeProxy createGenotype();
		Spec::PhenotypeProxy transform(Spec::GenotypeProxy);
		Spec::Fitness evaluate(Spec::GenotypeProxy);
		Spec::IndividualPtrs parentSelection(Spec::IndividualPtrs);
		Spec::GenotypeProxies variation(Spec::GenotypeProxies);
		void survivorSelection(Spec::IndividualPtrs &);
		bool convergenceCheck(Spec::Fitness);
};
