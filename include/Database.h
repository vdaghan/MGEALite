#pragma once

#include <filesystem>
#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

#include "SimulationLog.h"

using Generation = std::size_t;
using SimulationId = std::size_t;
using UpdatedSimulationList = std::list<SimulationInfo>;

enum class GenerationStatus {Exists, Unavailable};

class Database {
	public:
		Database(std::string);
		~Database();
		void rescan(bool once);
		std::size_t createNewGeneration();
		/// \brief Find existing simulation, or return nullptr if it does not exist.
		/// Locks database mutex while searching.
		/// \return
		SimulationLogPtr getSimulation(SimulationInfo);
		/// \brief Find existing simulation, or create and return one if it does not exist.
		/// Locks database mutex while searching and/or creating.
		/// \return
		SimulationLogPtr createSimulationInThisGeneration();
		UpdatedSimulationList getUpdatedSimulations();

		bool generationExists(std::size_t) const;
		std::optional<std::size_t> getCurrentGeneration() const;
		std::size_t getNextGeneration() const;

		bool simulationExists(SimulationInfo) const;

		SimulationLogPtrs & getGenerationData(std::size_t gen) { return simulations.at(gen); };
	private:
		std::filesystem::path const path;
		std::vector<SimulationLogPtrs> simulations;

		std::mutex dbMutex;
		std::jthread rescanThread;
		bool scan;
		UpdatedSimulationList updatedSimulations;

		SimulationLogPtr createSimulation(SimulationInfo);

		std::optional<std::size_t> maxSimulationId;
		void updateMaxSimulationId(std::size_t);
};
