#pragma once

#include "DEvA/Error.h"
#include "DEvA/JSON/Common.h"

#include "MotionGeneration/MotionParameters.h"

template <>
struct nlohmann::adl_serializer<ContactParameters> {
    static ContactParameters from_json(JSON const & j) {
		ContactParameters cP(defaultContactParameters());
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
		return cP;
    }
    static void to_json(JSON & j, ContactParameters const & cP) {
		j["stiffness"] = cP.stiffness;
		j["damping"] = cP.damping;
		j["transitionRegionWidth"] = cP.transitionRegionWidth;
		j["staticFriction"] = cP.staticFriction;
		j["dynamicFriction"] = cP.dynamicFriction;
		j["criticalVelocity"] = cP.criticalVelocity;
    }
};

