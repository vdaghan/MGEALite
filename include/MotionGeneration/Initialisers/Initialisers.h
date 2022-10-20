#pragma once

#include "Database.h"
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
		Database & database;
	};

	SimulationDataPtrs genesisBoundary(InitialiserParams);
	SimulationDataPtrs genesisBoundaryWavelet(InitialiserParams);
	SimulationDataPtrs genesisRandom(std::size_t, InitialiserParams);
	SimulationDataPtrs genesisZero(InitialiserParams);
}
