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
		std::vector<double> & time;
		MotionParameters & motionParameters;
		std::atomic<bool> & pauseFlag;
		std::atomic<bool> & stopFlag;
		Database & database;
	};

	Spec::GenotypeProxies genesisRandom(std::size_t, InitialiserParams);
	Spec::GenotypeProxies genesisBoundary(InitialiserParams);
	Spec::GenotypeProxies genesisBoundaryWavelet(InitialiserParams);
}
