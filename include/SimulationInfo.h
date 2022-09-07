#pragma once

#include <memory>

struct SimulationInfo {
	std::size_t generation;
	std::size_t identifier;
	friend bool operator<(SimulationInfo const & l, SimulationInfo const & r) {
		return std::tie(l.generation, l.identifier) < std::tie(r.generation, r.identifier);
	}
	friend bool operator>(SimulationInfo const & lhs, SimulationInfo const & rhs) {
		return rhs < lhs;
	}
	friend bool operator<=(SimulationInfo const & lhs, SimulationInfo const & rhs) {
		return !(lhs > rhs);
	}
	friend bool operator>=(SimulationInfo const & lhs, SimulationInfo const & rhs) {
		return !(lhs < rhs);
	}
	friend bool operator==(SimulationInfo const & lhs, SimulationInfo const & rhs) {
		return lhs.generation == rhs.generation and lhs.identifier == rhs.identifier;
	}
	friend bool operator!=(SimulationInfo const & lhs, SimulationInfo const & rhs) {
		return !(lhs == rhs);
	}
};
using SimulationInfoPtr = std::shared_ptr<SimulationInfo>;
