#include "SimulationData.h"

#include <spdlog/spdlog.h>

#include <fstream>


void updateSimulationDataPtr(SimulationDataPtrPair pair) {
	if (!pair.source) {
		return;
	}
	if (!pair.source->time.empty()) {
		pair.target->time = pair.source->time;
	}
	if (!pair.source->params.empty()) {
		pair.target->params = pair.source->params;
	}
	if (!pair.source->torque.empty()) {
		pair.target->torque = pair.source->torque;
	}
	if (!pair.source->outputs.empty()) {
		pair.target->outputs = pair.source->outputs;
	}
	pair.target->fitness = pair.source->fitness;
	if (pair.source->error) {
		pair.target->error = pair.source->error;
	}
}

void to_json(JSON & j, SimulationData const & sI) {
	if (sI.error) {
		j = JSON{{"time", sI.time},
				{"params", sI.params},
				{"torque", sI.torque},
				{"outputs", sI.outputs},
				{"fitness", sI.fitness},
				{"error", *sI.error}
		};
	} else {
		j = JSON{{"time", sI.time},
			{"params", sI.params},
			{"torque", sI.torque},
			{"outputs", sI.outputs},
			{"fitness", sI.fitness}
		};
	}
}

void from_json(JSON const & j, SimulationData & sI) {
	if (j.contains("time")) {
		j.at("time").get_to(sI.time);
	}
	if (j.contains("params")) {
		j.at("params").get_to(sI.params);
	}
	if (j.contains("torque")) {
		j.at("torque").get_to(sI.torque);
	}
	if (j.contains("outputs")) {
		auto & outputs = j["outputs"];
		for (auto outputIt = outputs.begin(); outputIt != outputs.end(); ++outputIt) {
			std::string outputKey = outputIt.key();
			auto & outputValue = outputIt.value();
			if (outputValue.is_array()) {
				auto outputValueAsArray = outputValue.get<std::vector<double>>();
				sI.outputs.emplace(std::make_pair(outputKey, outputValueAsArray));
			} else if (outputValue.is_number()) {
				auto outputValueAsNumber = outputValue.get<double>();
				std::vector<double> outputValueAsArray{outputValueAsNumber};
				sI.outputs.emplace(std::make_pair(outputKey, outputValueAsArray));
			}
		}
	}
	if (j.contains("fitness")) {
		j.at("fitness").get_to(sI.fitness);
	}
	if (j.contains("error")) {
		j.at("error").get_to(sI.error);
	}
}

MaybeSimulationDataPtr importSimulationData(std::filesystem::path fileName) {
	if (!std::filesystem::exists(fileName)) {
		return std::unexpected(MGEA::ErrorCode::FileNotFound);
	}
	std::ifstream f;
	f.open(fileName, std::ios_base::in);
	if (!f.is_open()) {
		return std::unexpected(MGEA::ErrorCode::ErrorReadingFile);
	}
	SimulationData parseResult;
	try {
		parseResult = nlohmann::json::parse(f);
	} catch (const std::exception &) {
		spdlog::warn("Could not parse simulation file {}. Will wait a bit more.", fileName.string());
		f.close();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return std::unexpected(MGEA::ErrorCode::ErrorReadingFile);
	}
	f.close();
	SimulationData * rawPtr = new SimulationData(parseResult);
	return SimulationDataPtr(rawPtr);
}

MGEA::ErrorCode exportSimulationData(SimulationDataPtr simulationDataPtr, std::filesystem::path fileName) {
	JSON j = *simulationDataPtr;
	std::ofstream o(fileName);
	o << std::setw(4) << j << std::endl;
	//o << j << std::endl;
	// TODO: Some error checking here
	return MGEA::ErrorCode::OK;
}
