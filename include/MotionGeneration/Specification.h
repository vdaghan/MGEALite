#pragma once

#include "Common.h"
#include "DEvA/EvolutionaryAlgorithm.h"
#include "DEvA/Specialisation.h"

#include "Metric.h"
#include "SimulationData.h"

class Database;
struct Specification {
	Specification(Database & database_) : database{database_} {};
	Database & database;
	using Genotype = SimulationDataPtr;
	using GenotypeProxy = DEvA::IndividualIdentifier;
	using Phenotype = SimulationDataPtr;
	using PhenotypeProxy = DEvA::IndividualIdentifier;
	using MetricVariant = MGEAMetricVariant;
	using Distance = double; // TODO: Distance is a metric
	using IndividualParameters = DEvA::NullVParameters;
};
using Spec = DEvA::Specialisation<Specification>;
