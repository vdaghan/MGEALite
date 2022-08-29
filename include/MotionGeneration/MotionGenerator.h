#pragma once

#include "SimulationData.h"

using Genotype = SimulationData;
using Phenotype = SimulationData;

#include "EvolutionaryAlgorithm.h"
#include "Specialisation.h"
using Fitness = double;
using Spec = DEvA::Specialisation<Genotype, Phenotype, Fitness, DEvA::NullVParameters>;

class MotionGenerator {
	public:
		MotionGenerator();
	private:
		DEvA::EvolutionaryAlgorithm<Spec> ea;
		Spec::Generation genesis();
		//Spec::GenotypePtr createGenotype();
		Spec::PhenotypePtr transform(Spec::GenotypePtr);
		Spec::Fitness evaluate(Spec::GenotypePtr);
		Spec::IndividualPtrs parentSelection(Spec::IndividualPtrs);
		Spec::GenotypePtrs variation(Spec::GenotypePtrs);
		void survivorSelection(Spec::IndividualPtrs &);
		bool convergenceCheck(Spec::Fitness);
};
