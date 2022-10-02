#include "Database.h"

#include "Logging/SpdlogCommon.h"

#include <algorithm>
#include <iostream>
#include <ranges>

Database::Database(std::string pathName, MotionParameters & mP) : motionParameters(mP), datastore(pathName), m_nextId(0) {
	syncWithDatastore();
	auto const & datastoreHistory = datastore.history();
	for (auto const & simInfo : datastoreHistory) {
		SimulationLogPtr simulationLogPtr = SimulationLogPtr(new SimulationLog(simInfo));
		simulationLogPtr->updateStatus(SimulationStatus::Archived);
		simulationHistory.emplace(std::make_pair(simInfo, simulationLogPtr));
	}
	startSyncLoop();
}

Database::~Database() {
	stopSyncLoop();
}

void Database::startSyncLoop() {
	keepSyncing = true;
	auto scanThreadLambda = [this]() {
		while (keepSyncing) {
			syncWithDatastore();
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	};
	scanThread = std::jthread(scanThreadLambda);
}

void Database::stopSyncLoop() {
	keepSyncing = false;
}

SimulationStatus Database::status(SimulationInfo simInfo) const {
	if (!simulationHistory.contains(simInfo)) {
		return SimulationStatus::NonExistent;
	}
	return simulationHistory.at(simInfo)->status();
}

SimulationDataPtr Database::createSimulation(SimulationInfo simInfo) {
	if (SimulationStatus::NonExistent != status(simInfo)) {
		return getSimulationLog(simInfo)->data();
	}
	SimulationLogPtr simulationLogPtr = SimulationLogPtr(new SimulationLog(simInfo));
	simulationLogPtr->data()->alignment = motionParameters.alignment;
	simulationLogPtr->data()->timeout = motionParameters.timeout;
	simulationLogPtr->data()->contacts = motionParameters.contactParameters;
	auto emplaceResult = simulationHistory.emplace(std::make_pair(simInfo, simulationLogPtr));
	if (!simulationHistory.contains(simInfo)) {
		return nullptr;
	}
	++m_nextId;
	simulationLogPtr->updateStatus(SimulationStatus::Uninitialised);
	addToList(uninitialised, simInfo);
	return simulationLogPtr->data();
}

MGEA::ErrorCode Database::startSimulation(SimulationInfo simInfo) {
	SimulationLogPtr simulationLogPtr = getSimulationLog(simInfo);
	if (!simulationLogPtr) {
		return MGEA::ErrorCode::Fail;
	}
	if (simulationLogPtr->inputExists()) {
		return MGEA::ErrorCode::OK;
	}
	simulationLogPtr->updateStatus(SimulationStatus::PendingSimulation);
	moveFromListToList(uninitialised, pendingSimulation, simInfo);
	return datastore.exportInputFile(simulationLogPtr);
}

MaybeSimulationDataPtr Database::getSimulationResult(SimulationInfo simInfo) {
	SimulationLogPtr simulationLogPtr = getSimulationLog(simInfo);
	if (!simulationLogPtr) {
		return nullptr;
	}
	if (simulationLogPtr->outputExists()) {
		return simulationLogPtr->data();
	}
	MGEA::ErrorCode importError = datastore.importOutputFile(simulationLogPtr);
	if (MGEA::ErrorCode::OK != importError) {
		return std::unexpected(importError);
	}
	if (simulationLogPtr->data()->error) {
		spdlog::warn("Simulation error for {}. Error message was:", simInfo);
		spdlog::warn("{}", *simulationLogPtr->data()->error);
		simulationLogPtr->updateStatus(SimulationStatus::SimulationError);
		MGEA::ErrorCode setError = datastore.setFitnessAndCombineFiles(simulationLogPtr, std::numeric_limits<double>::min());
		if (MGEA::ErrorCode::OK != setError) {
			return std::unexpected(setError);
		}
		return std::unexpected(MGEA::ErrorCode::SimulationError);
	}
	simulationLogPtr->updateStatus(SimulationStatus::PendingEvaluation);
	moveFromListToList(pendingSimulation, pendingEvaluation, simInfo);
	return simulationLogPtr->data();
}

MGEA::ErrorCode Database::setSimulationFitness(SimulationInfo simInfo, double fitness) {
	SimulationLogPtr simulationLogPtr = getSimulationLog(simInfo);
	if (!simulationLogPtr) {
		return MGEA::ErrorCode::Fail;
	}
	if (simulationLogPtr->fitnessExists()) {
		return MGEA::ErrorCode::OK;
	}
	MGEA::ErrorCode setError = datastore.setFitnessAndCombineFiles(simulationLogPtr, fitness);
	if (MGEA::ErrorCode::OK != setError) {
		return setError;
	}
	simulationLogPtr->updateStatus(SimulationStatus::Computed);
	moveFromListToList(pendingEvaluation, computed, simInfo);
	return MGEA::ErrorCode::OK;
}

SimulationLogPtr Database::getSimulationLog(SimulationInfo simInfo) {
	if (!simulationHistory.contains(simInfo)) {
		return nullptr;
	}
	return simulationHistory.at(simInfo);
}

SimulationHistory const & Database::getSimulationHistory() {
	return simulationHistory;
}

MGEA::ErrorCode Database::saveVisualisationTarget(SimulationInfo simInfo) {
	return datastore.saveVisualisationTarget(simInfo);
}

void Database::syncWithDatastore() {
	datastore.syncWithFilesystem();
}

bool Database::listContains(SimulationInfoList & list, SimulationInfo simInfo) {
	auto listIt = std::find(list.begin(), list.end(), simInfo);
	return listIt == list.end();
}

void Database::removeFromList(SimulationInfoList & list, SimulationInfo simInfo) {
	list.remove(simInfo);
}

void Database::addToList(SimulationInfoList & list, SimulationInfo simInfo) {
	list.push_back(simInfo);
	list.sort();
}

void Database::moveFromListToList(SimulationInfoList & source, SimulationInfoList & target, SimulationInfo simInfo) {
	removeFromList(source, simInfo);
	addToList(target, simInfo);
}
