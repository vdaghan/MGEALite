#pragma once

#include "Common.h"
#include "MotionGeneration/Specification.h"

#include <any>

namespace MGEA {
	JSON orderedVectorConversion(std::any value);

	OrderedVector angularVelocitySign(Spec::IndividualPtr);
	bool angularVelocitySignEquivalent(std::any lhs, std::any rhs);

	double angleDifferenceSum(Spec::IndividualPtr);
}
