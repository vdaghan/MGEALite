#pragma once

#include <filesystem>
#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <thread>

#include "IndividualIdentifier.h"

#include "Datastore.h"
#include "MotionGeneration/MotionParameters.h"
#include "MotionGeneration/Specification.h"
#include "SimulationLog.h"
#include "Error.h"
#include "MGEAError.h"

using Generation = std::size_t;
using SimulationId = std::size_t;
using UpdatedSimulationList = std::list<DEvA::IndividualIdentifier>;

using SimulationInfoList = std::set<DEvA::IndividualIdentifier>;
using SimulationHistory = std::map<DEvA::IndividualIdentifier, SimulationLogPtr>;
class Database {
	public:
		Database(std::string, MotionParameters &);
		~Database();

		void startSyncLoop();
		void stopSyncLoop();

		[[nodiscard]] SimulationStatus status(DEvA::IndividualIdentifier) const;
		[[nodiscard]] SimulationLogPtr registerSimulation(DEvA::IndividualIdentifier);
		[[nodiscard]] MGEA::ErrorCode startSimulation(DEvA::IndividualIdentifier);
		std::shared_future<MaybeSimulationDataPtr> requestSimulationResult(DEvA::IndividualIdentifier simInfo);
		MGEA::ErrorCode saveSimulationMetrics(DEvA::IndividualIdentifier, Spec::MetricVariantMap);
		[[nodiscard]] SimulationLogPtr getSimulationLog(DEvA::IndividualIdentifier);

		[[nodiscard]] SimulationHistory const & getSimulationHistory();

		MGEA::ErrorCode saveVisualisationTarget(DEvA::IndividualIdentifier);
	private:
		MotionParameters & motionParameters;
		Datastore datastore;
		mutable std::recursive_mutex dbMutex;
		void syncWithDatastore();
		bool keepSyncing;
		std::jthread scanThread;

		mutable std::recursive_mutex listMutex;
		SimulationInfoList uninitialised;
		SimulationInfoList pendingSimulation;
		SimulationInfoList pendingEvaluation;
		SimulationInfoList computed;

		mutable std::mutex promiseMutex;
		std::map<DEvA::IndividualIdentifier, std::promise<MaybeSimulationDataPtr>> simulationResultPromises;
		bool fullfillSimulationResultPromise(DEvA::IndividualIdentifier);
		bool listContains(SimulationInfoList &, DEvA::IndividualIdentifier);
		void removeFromList(SimulationInfoList &, DEvA::IndividualIdentifier);
		void addToList(SimulationInfoList &, DEvA::IndividualIdentifier);
		void moveFromListToList(SimulationInfoList &, SimulationInfoList &, DEvA::IndividualIdentifier);

		SimulationHistory simulationHistory;
};
