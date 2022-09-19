#include "SimulationLog.h"

#include "spdlog/spdlog.h"

SimulationLog::SimulationLog(SimulationInfo info)
	: m_info(info),
	m_status{SimulationStatus::Uninitialised},
	m_data(new SimulationData())
{
	touch();
}

bool SimulationLog::inputExists() const {
	return m_status == SimulationStatus::PendingSimulation
		or m_status == SimulationStatus::PendingEvaluation
		or m_status == SimulationStatus::Computed
		or m_status == SimulationStatus::Archived;
}

bool SimulationLog::outputExists() const {
	return m_status == SimulationStatus::PendingEvaluation
		or m_status == SimulationStatus::Computed
		or m_status == SimulationStatus::Archived;
}

bool SimulationLog::fitnessExists() const {
	return m_status == SimulationStatus::Computed
		or m_status == SimulationStatus::Archived;
}

bool SimulationLog::errorExists() const {
	return m_status == SimulationStatus::SimulationError;
}

uint64_t SimulationLog::timeSinceLastTouch() {
	auto currentTimepoint = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(currentTimepoint - m_lastTouch).count();
}

void SimulationLog::touch() {
	m_lastTouch = std::chrono::steady_clock::now();
}
