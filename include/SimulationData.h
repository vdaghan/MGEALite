#pragma once

#include "MGEAError.h"

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using JSON = nlohmann::ordered_json;

struct SimulationData {
	std::vector<double> time;
	std::map<std::string, double> params;
	std::map<std::string, std::vector<double>> torque;
	std::map<std::string, std::vector<double>> outputs;
	double fitness;
	std::optional<std::string> error;
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
