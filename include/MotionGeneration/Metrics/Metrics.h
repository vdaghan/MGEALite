#pragma once

#include "Common.h"
#include "MotionGeneration/Specification.h"

namespace MGEA {
	OrderedVector computeAngularVelocitySign(VectorMap const & angles);
	OrderedVector angularVelocitySign(Spec::IndividualPtr);
}
