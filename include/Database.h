#pragma once

#include <filesystem>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <thread>

#include "SimulationLog.h"

using Generation = std::size_t;
using SimulationId = std::size_t;
struct SimulationInfo {
	std::size_t generation;
	std::size_t identifier;
};
using SimulationInfoPtr = std::shared_ptr<SimulationInfo>;
using UpdatedSimulationList = std::list<SimulationInfo>;

class Database {
	public:
		Database(std::string);
		~Database();
		void rescan();
		/// \brief Find existing simulation, or return nullptr if it does not exist.
		/// Locks database mutex while searching.
		/// \return
		SimulationLogPtr getSimulation(SimulationInfo);
		/// \brief Find existing simulation, or create and return one if it does not exist.
		/// Locks database mutex while searching and/or creating.
		/// \return
		SimulationLogPtr createSimulation(SimulationInfo);
		UpdatedSimulationList getUpdatedSimulations();
	private:
		std::filesystem::path const path;
		std::map<Generation, SimulationLogPtrMap> simulations;

		std::mutex dbMutex;
		std::jthread rescanThread;
		bool scan;
		UpdatedSimulationList updatedSimulations;
};
