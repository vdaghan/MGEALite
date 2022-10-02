#pragma once

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
	std::vector<double> time;
	std::map<std::string, double> params;
	std::optional<int> alignment;
	std::optional<double> timeout;
	std::map<std::string, std::vector<double>> torque;
	std::optional<ContactParameters> contacts;
	std::map<std::string, std::vector<double>> outputs;
	std::map<std::string, double> metadata;
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
