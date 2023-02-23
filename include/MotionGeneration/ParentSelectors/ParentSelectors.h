#pragma once

#include "MotionGeneration/MotionParameters.h"
#include "MotionGeneration/SharedSynchronisationHelpers.h"
#include "MotionGeneration/Specification.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <utility>
#include <vector>

namespace MGEA {
	Spec::IndividualPtrs metricProportional(DEvA::ParameterMap parameters, Spec::IndividualPtrs domain);
}
