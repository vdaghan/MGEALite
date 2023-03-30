#pragma once

#include "MotionGeneration/MotionParameters.h"
#include "MotionGeneration/SharedSynchronisationHelpers.h"
#include "MotionGeneration/Specification.h"

#include <algorithm>
#include <atomic>
#include <utility>
#include <vector>

namespace MGEA {
	struct InitialiserParams {
		MotionParameters & motionParameters;
		std::atomic<bool> & pauseFlag;
		std::atomic<bool> & stopFlag;
	};

	SimulationDataPtrs genesisBoundary(MotionParameters, DEvA::ParameterMap);
	SimulationDataPtrs genesisBoundaryWavelet(MotionParameters, DEvA::ParameterMap);
	SimulationDataPtrs genesisImportFromFolder(MotionParameters, DEvA::ParameterMap);
	SimulationDataPtrs genesisRandom(MotionParameters, DEvA::ParameterMap);
	SimulationDataPtrs genesisZero(MotionParameters, DEvA::ParameterMap);
}
