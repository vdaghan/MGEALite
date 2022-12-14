#pragma once

#include "MGEAError.h"
#include "SimulationLog.h"

#include "DEvA/IndividualIdentifier.h"
#include "MotionGeneration/Specification.h"

#include <atomic>
#include <deque>
#include <filesystem>
#include <list>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

using DatastoreGeneration = std::list<DEvA::IndividualIdentifier>;
using DatastoreHistory = std::deque<DatastoreGeneration>;
using DatastoreHistoryIterator = decltype(std::declval<DatastoreHistory>().begin());

struct Progress {
	std::list<std::size_t> idsWithInputs;
	std::list<std::size_t> idsWithOutputs;
	std::list<std::size_t> idsWithInputsAndOutputs;
};

struct SimulationFile {
	std::size_t id;
	std::string type;
};

class Datastore {
	public:
		Datastore(std::filesystem::path);
		~Datastore();

		Spec::MaybePhenotype simulate(Spec::Genotype);
	private:
		std::atomic<std::size_t> simId;
		std::filesystem::path const m_path;
		std::filesystem::path const m_queuePath;
		std::filesystem::path const m_visualisationPath;
		std::list<SimulationFile> scanFiles();
		Progress getProgress(std::list<SimulationFile>);
		std::mutex promiseMutex;
		std::map<std::size_t, std::promise<Spec::MaybePhenotype>> simulationResultPromises;
		std::list<std::size_t> fulfillPromises(Progress);
		std::list<std::filesystem::path> getObsoleteFiles(std::list<std::size_t>);
		void updateDeleteQueue(std::list<std::filesystem::path>);
		std::list<std::filesystem::path> deleteQueue;
		void deleteFiles();
		void syncWithFilesystem();

		void startSyncLoop();
		void stopSyncLoop();
		bool keepSyncing;
		std::jthread syncThread;
		template <typename L>
		void unique(L & list) {
			std::stable_sort(list.begin(), list.end());
			auto last = std::unique(list.begin(), list.end());
			list.erase(last, list.end());
		}

		std::filesystem::path toInputPath(std::size_t);
		std::filesystem::path toOutputPath(std::size_t);
		std::filesystem::path toGenerationPath(DEvA::IndividualIdentifier);

		//MGEA::ErrorCode saveVisualisationTarget(DEvA::IndividualIdentifier);
};
