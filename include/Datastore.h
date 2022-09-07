#pragma once

#include "SimulationLog.h"

#include <filesystem>
#include <list>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

using History = std::vector<std::list<SimulationInfo>>;
using HistoryIterator = decltype(std::declval<History>().begin());

class Datastore {
	public:
		Datastore(std::filesystem::path);
		bool pushToSimulationQueue(SimulationLogPtr);
		SimulationLogPtr popFromEvaluationQueue();
		bool combineInputOutputAndFitness(SimulationInfo, double);

		History const & history() const { return m_history; };
	private:
		std::filesystem::path const m_path;
		std::filesystem::path const m_queuePath;
		std::mutex dsMutex;

		std::list<std::size_t> simulationQueue;
		std::list<std::size_t> evaluationQueue;
		std::list<std::size_t> deleteQueue;
		bool deleteUncombined(std::size_t);
		History m_history;
		bool existsInHistory(SimulationInfo);
		void addToHistory(SimulationInfo);

		std::filesystem::path toInputPath(std::size_t);
		std::filesystem::path toOutputPath(std::size_t);
		std::filesystem::path toCombinedPath(SimulationInfo);
		
		void sync();
};
