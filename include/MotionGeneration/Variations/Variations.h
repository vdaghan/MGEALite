#pragma once

#include "MotionGeneration/MotionParameters.h"
#include "MotionGeneration/Specification.h"
#include "SimulationData.h"

#include "DEvA/EvolutionaryAlgorithm.h"

namespace MGEA {
	struct VariationParams {
		MotionParameters& motionParameters;
		std::atomic<bool>& pauseFlag;
		std::atomic<bool>& stopFlag;
	};

	SimulationDataPtrs crossoverAll(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs crossoverSingle(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs cutAndCrossfillAll(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs cutAndCrossfillSingle(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs deletionAll(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs deletionSingle(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs insertionAll(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs insertionSingle(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs kPointCrossoverAll(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs kPointCrossoverSingle(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs snv(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs uniformCrossoverAll(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs uniformCrossoverSingle(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs waveletSNV(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs halfSineAsynchronous(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs halfSineSingle(VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs halfSineSynchronous(VariationParams, Spec::IndividualPtrs);

	SimulationDataPtrs deletionLInt(std::size_t, VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs directionalLInt(std::size_t, VariationParams, Spec::IndividualPtrs);
	SimulationDataPtrs snvLInt(std::size_t, VariationParams, Spec::IndividualPtrs);
}
