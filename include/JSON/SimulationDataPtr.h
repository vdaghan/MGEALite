#pragma once

#include "DEvA/Error.h"
#include "DEvA/JSON/Common.h"

#include "SimulationData.h"

template <>
struct nlohmann::adl_serializer<SimulationDataPtr> {
    static SimulationDataPtr from_json(JSON const & j) {
        SimulationDataPtr sptr = std::make_shared<SimulationData>();
		if (j.contains("time")) {
			j.at("time").get_to(sptr->time);
		}
		if (j.contains("alignment")) {
			sptr->alignment = j.at("alignment").get<int>();
		}
		if (j.contains("timeout")) {
			sptr->timeout = j.at("timeout").get<double>();
		}
		if (j.contains("params")) {
			j.at("params").get_to(sptr->params);
		}
		if (j.contains("masses")) {
			j.at("masses").get_to(sptr->masses);
		}
		if (j.contains("metadata")) {
			j.at("metadata").get_to(sptr->metadata);
		}
		if (j.contains("contacts")) {
			sptr->contacts = j.at("contacts").get<ContactParameters>();
		}
		if (j.contains("torque")) {
			j.at("torque").get_to(sptr->torque);
		}
		if (j.contains("error")) {
			j.at("error").get_to(sptr->error);
			return sptr;
		}
		if (j.contains("angles")) {
			j.at("angles").get_to(sptr->angles);
		}
		if (j.contains("outputs")) {
			auto & outputs = j["outputs"];
			for (auto outputIt = outputs.begin(); outputIt != outputs.end(); ++outputIt) {
				std::string outputKey = outputIt.key();
				auto & outputValue = outputIt.value();
				if (outputValue.is_array()) {
					auto outputValueAsArray = outputValue.get<std::vector<double>>();
					sptr->outputs.emplace(std::make_pair(outputKey, outputValueAsArray));
				} else if (outputValue.is_number()) {
					auto outputValueAsNumber = outputValue.get<double>();
					std::vector<double> outputValueAsArray{outputValueAsNumber};
					sptr->outputs.emplace(std::make_pair(outputKey, outputValueAsArray));
				}
			}
		}
		if (j.contains("metrics")) {
			sptr->metrics = j.at("metrics").get<std::string>();
		}
		return sptr;
    }

    static void to_json(JSON & j, SimulationDataPtr const & sptr) {
		if (!sptr) {
			return;
		}
		if (!sptr->time.empty()) {
			j["time"] = sptr->time;
		}
		if (sptr->alignment) {
			j["alignment"] = sptr->alignment.value();
		}
		if (sptr->timeout) {
			j["timeout"] = sptr->timeout.value();
		}
		if (!sptr->params.empty()) {
			j["params"] = sptr->params;
		}
		if (!sptr->masses.empty()) {
			j["masses"] = sptr->masses;
		}
		if (!sptr->metadata.empty()) {
			j["metadata"] = sptr->metadata;
		}
		if (sptr->contacts) {
			j["contacts"] = sptr->contacts.value();
		}
		if (!sptr->torque.empty()) {
			j["torque"] = sptr->torque;
		}
		if (!sptr->angles.empty()) {
			j["angles"] = sptr->angles;
		}
		if (!sptr->outputs.empty()) {
			j["outputs"] = sptr->outputs;
		}
		if (sptr->metrics) {
			j["metrics"] = sptr->metrics.value();
		}
		if (sptr->error) {
			j["error"] = sptr->error.value();
		}
    }
};

namespace MGEA {
	[[nodiscard]] DEvA::Maybe<SimulationDataPtr> importSimulationDataPtrFromFile(std::filesystem::path const & filename);
	DEvA::ErrorCode exportSimulationDataPtrToFile(SimulationDataPtr const & t, std::filesystem::path const & filename);
}
