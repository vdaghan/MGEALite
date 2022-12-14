// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "SimulationData.h"

#include <spdlog/spdlog.h>

#include <fstream>

bool SimulationData::valid() {
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
}

void SimulationData::updateUsing(SimulationData const & other) {
	if (!other.time.empty()) {
		time = other.time;
	}
	if (other.alignment) {
		alignment = other.alignment;
	}
	if (other.timeout) {
		timeout = other.timeout;
	}
	if (!other.params.empty()) {
		params = other.params;
	}
	if (!other.masses.empty()) {
		masses = other.masses;
	}
	if (!other.metadata.empty()) {
		metadata = other.metadata;
	}
	if (other.contacts) {
		contacts = other.contacts;
	}
	if (!other.torque.empty()) {
		torque = other.torque;
	}
	if (!other.angles.empty()) {
		angles = other.angles;
	}
	if (!other.outputs.empty()) {
		outputs = other.outputs;
	}
	metrics = other.metrics;
	if (other.error) {
		error = other.error;
	}
}
