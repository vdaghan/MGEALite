#pragma once

#include "MotionGeneration/MotionParameters.h"
#include "MotionGeneration/Specification.h"
#include "SimulationData.h"

#include "DEvA/EvolutionaryAlgorithm.h"

namespace MGEA {
	SimulationDataPtrs crossoverAll(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs crossoverSingle(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs cutAndCrossfillAll(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs cutAndCrossfillSingle(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs deletionAll(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs deletionSingle(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs insertionAll(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs insertionSingle(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs kPointCrossoverAll(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs kPointCrossoverSingle(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs snv(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs uniformCrossoverAll(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs uniformCrossoverSingle(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs waveletSNV(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs halfSineAsynchronous(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs halfSineSingle(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs halfSineSynchronous(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);

	SimulationDataPtrs deletionLInt(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs directionalLInt(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
	SimulationDataPtrs snvLInt(MotionParameters, DEvA::ParameterMap, Spec::IndividualPtrs);
}
