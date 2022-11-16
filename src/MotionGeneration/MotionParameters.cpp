#include "MotionGeneration/MotionParameters.h"

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

void to_json(JSON & j, ContactParameters const & cP) {
	j["stiffness"] = cP.stiffness;
	j["damping"] = cP.damping;
	j["transitionRegionWidth"] = cP.transitionRegionWidth;
	j["staticFriction"] = cP.staticFriction;
	j["dynamicFriction"] = cP.dynamicFriction;
	j["criticalVelocity"] = cP.criticalVelocity;
}

void from_json(JSON const & j, ContactParameters & cP) {
	if (j.contains("stiffness")) {
		j.at("stiffness").get_to(cP.stiffness);
	}
	if (j.contains("damping")) {
		j.at("damping").get_to(cP.damping);
	}
	if (j.contains("transitionRegionWidth")) {
		j.at("transitionRegionWidth").get_to(cP.transitionRegionWidth);
	}
	if (j.contains("staticFriction")) {
		j.at("staticFriction").get_to(cP.staticFriction);
	}
	if (j.contains("dynamicFriction")) {
		j.at("dynamicFriction").get_to(cP.dynamicFriction);
	}
	if (j.contains("criticalVelocity")) {
		j.at("criticalVelocity").get_to(cP.criticalVelocity);
	}
}
