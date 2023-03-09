// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/ContactParameters.h"

ContactParameters defaultContactParameters() {
	return ContactParameters{
		.stiffness = 1e6,
		.damping = 1e3,
		.transitionRegionWidth = 1e-4,
		.staticFriction = 0.5,
		.dynamicFriction = 0.3,
		.criticalVelocity = 1e-3
	};
};

ContactParameters bodyGroundContactParameters() {
	return ContactParameters{
		.stiffness = 1e6,
		.damping = 1e5,
		.transitionRegionWidth = 1e-4,
		.staticFriction = 3.0,
		.dynamicFriction = 2.0,
		.criticalVelocity = 1e-3
	};
};
