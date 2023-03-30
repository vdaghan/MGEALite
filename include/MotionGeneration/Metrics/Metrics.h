#pragma once

#include "Common.h"
#include "MotionGeneration/Specification.h"

#include <any>

namespace MGEA {
	using SteppedDouble = std::pair<std::size_t, double>;

	JSON orderedVectorConversion(std::any value);

	JSON steppedDoubleConversion(std::any value);
	bool steppedDoubleEquivalence(std::any lhs, std::any rhs);
	bool steppedDoubleLesser(std::any lhs, std::any rhs);
	bool steppedDoubleGreater(std::any lhs, std::any rhs);

	std::any outputBetweenTwoValues(DEvA::ParameterMap, Spec::IndividualPtr);

	std::any angularVelocitySign(DEvA::ParameterMap, Spec::IndividualPtr);
	bool angularVelocitySignEquivalent(std::any lhs, std::any rhs);

	std::any angleDifferenceSum(DEvA::ParameterMap, Spec::IndividualPtr);
	std::any angleDifferenceSumLinearWeighted(DEvA::ParameterMap, Spec::IndividualPtr);
	std::any averageOfAngleDifferenceSumsStepped(DEvA::ParameterMap, Spec::IndividualPtr);
	std::any maximumAngleDifference(DEvA::ParameterMap, Spec::IndividualPtr);
	std::any maximumAngleDifferenceStepped(DEvA::ParameterMap, Spec::IndividualPtr);
}
