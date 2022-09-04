#pragma once

#include "Database.h"

struct Specification {
	Specification(Database& database_) : database{database_} {};
	Database& database;
	using Genotype = std::vector<size_t>;
	using GenotypeProxy = SimulationInfo;
	using Phenotype = std::vector<size_t>;
	using PhenotypeProxy = SimulationInfo;
	using Fitness = double;
	using IndividualParameters = DEvA::NullVParameters;
};
