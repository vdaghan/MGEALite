#pragma once

#include <chrono>
#include <deque>
#include <memory>

#include "IndividualIdentifier.h"

#include "SimulationData.h"

enum class SimulationStatus { NonExistent, Uninitialised, PendingSimulation, SimulationError, PendingEvaluation, Computed, Archived };

class SimulationLog {
	public:
		SimulationLog(DEvA::IndividualIdentifier);

		SimulationStatus status() const { return m_status; };
		void updateStatus(SimulationStatus status) { touch(); m_status = status; };

		std::size_t generation() const { return m_info.generation; };
		std::size_t identifier() const { return m_info.identifier; };
		DEvA::IndividualIdentifier info() const { return m_info; };

		bool inputExists() const;
		bool outputExists() const;
		bool fitnessExists() const;
		bool errorExists() const;

		SimulationDataPtr data() { touch(); return m_data; };
		uint64_t timeSinceLastTouch();
	private:
		DEvA::IndividualIdentifier m_info;
		SimulationStatus m_status;
		SimulationDataPtr m_data;

		std::chrono::time_point<std::chrono::steady_clock> m_lastTouch;
		void touch();
};

using SimulationLogPtr = std::shared_ptr<SimulationLog>;
using SimulationLogPtrs = std::deque<SimulationLogPtr>;
