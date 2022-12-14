#pragma once

#include "Metric.h"
#include "SimulationData.h"

#include "Common.h"
#include "DEvA/EvolutionaryAlgorithm.h"
#include "DEvA/Specialisation.h"
#include "JSON/SimulationDataPtr.h"

class Database;
struct Specification {
	Specification(Database & database_) : database{database_} {};
	Database & database;
	using Genotype = SimulationDataPtr;
	using Phenotype = SimulationDataPtr;
	using MetricVariant = MGEAMetricVariant;
};
using Spec = DEvA::Specialisation<Specification>;
