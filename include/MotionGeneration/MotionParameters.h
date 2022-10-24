#pragma once

#include <nlohmann/json.hpp>
using JSON = nlohmann::ordered_json;

#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

struct ContactParameters {
	double stiffness; // Default 1e6 N/m in MATLAB
	double damping; // Default 1e3 N(/m/S) in MATLAB
	double transitionRegionWidth; // Default 1e-4m in MATLAB
	double staticFriction; // Default 0.5 in MATLAB
	double dynamicFriction; // Default 0.3 in MATLAB
	double criticalVelocity; // Default 1e-3 m/s in MATLAB
};

ContactParameters defaultContactParameters();
ContactParameters bodyGroundContactParameters();
void to_json(JSON &, ContactParameters const &);
void from_json(JSON const &, ContactParameters &);

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
