#pragma once

#include "MotionGeneration/MotionParameters.h"

template <>
struct nlohmann::adl_serializer<MotionParameters> {
    static MotionParameters from_json(JSON const & j) {
        MotionParameters mP{};
        
		if (j.contains("simStart")) {
			j.at("simStart").get_to(mP.simStart);
		}
		if (j.contains("simStep")) {
			j.at("simStep").get_to(mP.simStep);
		}
		if (j.contains("simSamples")) {
			j.at("simSamples").get_to(mP.simSamples);
		}
		if (j.contains("alignment")) {
			j.at("alignment").get_to(mP.alignment);
		}
		if (j.contains("timeout")) {
			j.at("timeout").get_to(mP.timeout);
		}
		if (j.contains("masses")) {
			j.at("masses").get_to(mP.masses);
		}
		if (j.contains("jointNames")) {
			j.at("jointNames").get_to(mP.jointNames);
		}
		if (j.contains("jointLimits")) {
			j.at("jointLimits").get_to(mP.jointLimits);
		}
		if (j.contains("contactParameters")) {
			j.at("contactParameters").get_to(mP.contactParameters);
		}

        return mP;
    }
    static void to_json(JSON & j, MotionParameters const & mP) {
		j["stiffness"] = cP.stiffness;
    }
};
