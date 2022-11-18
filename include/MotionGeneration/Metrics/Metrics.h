#pragma once

#include "Common.h"
#include "MotionGeneration/Specification.h"

namespace MGEA {
	OrderedVector computeAngularVelocitySign(VectorMap angles);
	OrderedVector angularVelocitySign(Spec::IndividualPtr);
}
