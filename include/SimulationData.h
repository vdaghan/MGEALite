#pragma once

#include "Common.h"
#include "Metric.h"
#include "MGEAError.h"
#include "MotionGeneration/MotionParameters.h"

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using JSON = nlohmann::ordered_json;

struct SimulationData {
	MGEA::DataVector time;
	std::optional<int> alignment;
	std::optional<double> timeout;
	std::map<std::string, double> params;
	std::map<std::string, double> masses;
	std::map<std::string, double> metadata;
	std::optional<ContactParameters> contacts;
	MGEA::VectorMap torque;
	MGEA::VectorMap angles;
	MGEA::VectorMap outputs;
	std::optional<std::string> metrics;
	std::optional<std::string> error;
	bool valid() {
		if (error) {
			return false;
		}
		std::size_t const length(time.size());
		for (auto & t : torque) {
			if (length != t.second.size()) {
				return false;
			}
		}
		for (auto & o : outputs) {
			if (length != o.second.size()) {
				return false;
			}
		}
		return true;
	};
};

using SimulationDataPtr = std::shared_ptr<SimulationData>;
using MaybeSimulationDataPtr = MGEA::Maybe<SimulationDataPtr>;
using SimulationDataPtrs = std::list<SimulationDataPtr>;

struct SimulationDataPtrPair {
	SimulationDataPtr source;
	SimulationDataPtr target;
};
void updateSimulationDataPtr(SimulationDataPtrPair);

void to_json(JSON &, SimulationData const &);
void from_json(JSON const &, SimulationData &);
MaybeSimulationDataPtr importSimulationData(std::filesystem::path);
MGEA::ErrorCode exportSimulationData(SimulationDataPtr, std::filesystem::path);
