#pragma once

#include "EvolutionaryAlgorithm.h"

#include "Database.h"
#include "Specialisation.h"

struct Specification {
	Specification(Database & database_) : database{database_} {};
	Database & database;
	using Genotype = SimulationDataPtr;
	using GenotypeProxy = SimulationInfo;
	using Phenotype = SimulationDataPtr;
	using PhenotypeProxy = SimulationInfo;
	using Fitness = double;
	using Distance = std::size_t;
	using IndividualParameters = DEvA::NullVParameters;
};
using Spec = DEvA::Specialisation<Specification>;
