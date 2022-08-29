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
using UpdatedSimulationList = std::list<SimulationInfo>;

class Database {
	public:
		Database(std::string);
		~Database();
		void rescan();
		SimulationLogPtr getSimulation(std::size_t, std::size_t);
		SimulationLogPtr createSimulation(std::size_t, std::size_t);
		UpdatedSimulationList getUpdatedSimulations();
	private:
		std::filesystem::path const path;
		std::map<Generation, SimulationLogPtrMap> simulations;

		std::mutex dbMutex;
		std::jthread rescanThread;
		bool scan;
		UpdatedSimulationList updatedSimulations;
};
