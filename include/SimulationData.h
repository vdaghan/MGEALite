#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using JSON = nlohmann::ordered_json;

struct SimulationData {
	std::vector<double> time;
	std::map<std::string, double> params;
	std::map<std::string, std::vector<double>> torque;
	std::map<std::string, std::vector<double>> outputs;
};

using SimulationDataPtr = std::shared_ptr<SimulationData>;

void to_json(JSON &, SimulationData const &);
void from_json(JSON const &, SimulationData &);
SimulationDataPtr importSimulationData(std::filesystem::path);
void exportSimulationData(SimulationDataPtr, std::filesystem::path);
