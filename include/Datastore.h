#pragma once

#include "MGEAError.h"
#include "SimulationLog.h"

#include <filesystem>
#include <list>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

using DatastoreHistory = std::list<SimulationInfo>;
using DatastoreHistoryIterator = decltype(std::declval<DatastoreHistory>().begin());

struct Progress {
	std::list<std::size_t> idsWithInputs;
	std::list<std::size_t> idsWithOutputs;
	std::list<std::size_t> idsWithInputsAndOutputs;
};

class Datastore {
	public:
		Datastore(std::filesystem::path);
		~Datastore();

		void syncWithFilesystem();

		MGEA::ErrorCode exportInputFile(SimulationLogPtr);
		MGEA::ErrorCode importOutputFile(SimulationLogPtr);
		MGEA::ErrorCode setFitnessAndCombineFiles(SimulationLogPtr, double);
		MaybeSimulationDataPtr importCombinedFile(SimulationInfo);

		DatastoreHistory const & history() const { return m_history; };
		bool existsInHistory(SimulationInfo);
		MGEA::ErrorCode saveVisualisationTarget(SimulationInfo);
	private:
		std::filesystem::path const m_path;
		std::filesystem::path const m_queuePath;
		std::filesystem::path const m_visualisationPath;

		std::list<std::size_t> simulationQueue;
		std::list<std::size_t> evaluationQueue;
		std::list<std::size_t> deleteQueue;
		Progress getProgress();
		bool deleteArtifacts(std::size_t);
		void deleteAllArtifacts();
		DatastoreHistory m_history;
		void addToHistory(SimulationInfo);

		std::filesystem::path toInputPath(std::size_t);
		std::filesystem::path toOutputPath(std::size_t);
		std::filesystem::path toCombinedPath(SimulationInfo);
		std::filesystem::path toGenerationPath(SimulationInfo);
};
