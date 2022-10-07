#pragma once

#include "MotionGeneration/MotionParameters.h"
#include "SimulationData.h"

std::array<size_t, 2> stageToIndices(size_t numJoints, size_t stage);
SimulationDataPtrs waveletSNV(MotionParameters const &, SimulationDataPtrs);
