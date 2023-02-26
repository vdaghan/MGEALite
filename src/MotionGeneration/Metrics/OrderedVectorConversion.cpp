// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Metrics/Metrics.h"

#include <cmath>
#include <vector>

namespace MGEA {
	JSON orderedVectorConversion(std::any value) {
		return std::any_cast<MGEA::OrderedVector>(value);
	}
}