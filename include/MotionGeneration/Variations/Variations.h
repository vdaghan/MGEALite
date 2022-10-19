#pragma once

#include "Database.h"
#include "MotionGeneration/MotionParameters.h"
#include "SimulationData.h"

#include "EvolutionaryAlgorithm.h"

namespace MGEA {
	struct VariationParams {
		MotionParameters& motionParameters;
		std::atomic<bool>& pauseFlag;
		std::atomic<bool>& stopFlag;
	};

	SimulationDataPtrs crossoverAll(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs crossoverSingle(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs cutAndCrossfillAll(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs cutAndCrossfillSingle(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs deletionAll(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs deletionSingle(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs insertionAll(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs insertionSingle(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs kPointCrossoverAll(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs kPointCrossoverSingle(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs snv(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs uniformCrossoverAll(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs uniformCrossoverSingle(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs waveletSNV(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs halfSineSingle(VariationParams, SimulationDataPtrs);
	SimulationDataPtrs halfSineSynchronous(VariationParams, SimulationDataPtrs);
}
