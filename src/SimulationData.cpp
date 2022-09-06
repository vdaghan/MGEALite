#include "SimulationData.h"

#include <spdlog/spdlog.h>

#include <fstream>

void to_json(JSON & j, SimulationData const & sI) {
	j = JSON{{"time", sI.time},
			{"params", sI.params},
			{"torque", sI.torque},
			{"outputs", sI.outputs}
	};
}

void from_json(JSON const & j, SimulationData & sI) {
	if (j.contains("time")) {
		j.at("time").get_to(sI.time);
	} else if (j.contains("params")) {
		j.at("params").get_to(sI.params);
	} else if (j.contains("torque")) {
		j.at("torque").get_to(sI.torque);
	} else if (j.contains("outputs")) {
		j.at("outputs").get_to(sI.outputs);
	}
}

SimulationDataPtr importSimulationData(std::filesystem::path fileName) {
	if (!std::filesystem::exists(fileName)) {
		return nullptr;
	}
	std::ifstream f(fileName);
	if (!f.is_open()) {
		return nullptr;
	}
	SimulationData parseResult;
	try {
		parseResult = nlohmann::json::parse(f);
	} catch (const std::exception &) {
		spdlog::warn("Could not parse simulation file {}. Will wait a bit more.", fileName.string());
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return nullptr;
	}
	SimulationData * rawPtr = new SimulationData(parseResult);
	return SimulationDataPtr(rawPtr);
}

void exportSimulationData(SimulationDataPtr simulationDataPtr, std::filesystem::path fileName) {
	JSON j = *simulationDataPtr;
	std::ofstream o(fileName);
	o << std::setw(4) << j << std::endl;
	//o << j << std::endl;
}
