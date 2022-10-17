#pragma once

#include <filesystem>
#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

#include "Datastore.h"
#include "MotionGeneration/MotionParameters.h"
#include "SimulationLog.h"
#include "Error.h"
#include "MGEAError.h"

using Generation = std::size_t;
using SimulationId = std::size_t;
using UpdatedSimulationList = std::list<SimulationInfo>;

using SimulationInfoList = std::list<SimulationInfo>;
using SimulationHistory = std::map<SimulationInfo, SimulationLogPtr>;
class Database {
	public:
		Database(std::string, MotionParameters &);
		~Database();

		void startSyncLoop();
		void stopSyncLoop();

		[[nodiscard]] SimulationStatus status(SimulationInfo) const;
		[[nodiscard]] SimulationLogPtr registerSimulation(SimulationInfo);
		[[nodiscard]] MGEA::ErrorCode startSimulation(SimulationInfo);
		[[nodiscard]] MaybeSimulationDataPtr getSimulationResult(SimulationInfo);
		MGEA::ErrorCode setSimulationFitness(SimulationInfo, double);

		std::shared_future<MaybeSimulationDataPtr> requestSimulationResult(SimulationInfo simInfo);

		[[nodiscard]] SimulationLogPtr getSimulationLog(SimulationInfo);

		[[nodiscard]] SimulationHistory const & getSimulationHistory();

		MGEA::ErrorCode saveVisualisationTarget(SimulationInfo);
	private:
		MotionParameters & motionParameters;
		Datastore datastore;
		mutable std::recursive_mutex dbMutex;
		void syncWithDatastore();
		bool keepSyncing;
		std::jthread scanThread;

		SimulationInfoList uninitialised;
		SimulationInfoList pendingSimulation;
		SimulationInfoList pendingEvaluation;
		SimulationInfoList computed;

		mutable std::mutex promiseMutex;
		std::map<SimulationInfo, std::promise<MaybeSimulationDataPtr>> simulationResultPromises;
		bool fullfillSimulationResultPromise(SimulationInfo);
		bool listContains(SimulationInfoList &, SimulationInfo);
		void removeFromList(SimulationInfoList &, SimulationInfo);
		void addToList(SimulationInfoList &, SimulationInfo);
		void moveFromListToList(SimulationInfoList &, SimulationInfoList &, SimulationInfo);

		SimulationHistory simulationHistory;
};
