#pragma once

#include <nlohmann/json.hpp>
using JSON = nlohmann::ordered_json;

#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "MotionGeneration/ContactParameters.h"

struct MotionParameters {
	double simStart;
	double simStep;
	std::size_t simSamples;
	double simStop() { return simStart + simStep * static_cast<double>(simSamples - 1); };
	std::vector<double> time() {
		std::vector<double> timeVec(simSamples);
		std::generate(timeVec.begin(), timeVec.end(), [this, t = simStart]() mutable {
			return (t += simStep);
		});
		return timeVec;
	}
	int alignment;
	double timeout;
	std::map<std::string, double> masses;
	std::vector<std::string> jointNames;
	std::map<std::string, std::pair<double, double>> jointLimits;
	std::optional<ContactParameters> contactParameters;
};
