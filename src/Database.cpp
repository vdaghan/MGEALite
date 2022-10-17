#include "Database.h"

#include "Logging/SpdlogCommon.h"

#include <algorithm>
#include <iostream>
#include <ranges>

Database::Database(std::string pathName, MotionParameters & mP) : motionParameters(mP), datastore(pathName) {
	syncWithDatastore();
	auto const & datastoreHistory = datastore.history();
	for (auto const & datastoreHistoryGeneration : datastoreHistory) {
		for (auto const & simInfo : datastoreHistoryGeneration) {
			SimulationLogPtr simulationLogPtr = std::make_shared<SimulationLog>(simInfo);
			MaybeSimulationDataPtr sdPtr = datastore.importCombinedFile(simInfo);
			if (sdPtr.has_value()) {
				updateSimulationDataPtr(SimulationDataPtrPair{ .source = sdPtr.value(), .target = simulationLogPtr->data() });
				if (sdPtr.value()->error) {
					simulationLogPtr->updateStatus(SimulationStatus::SimulationError);
				}
				else {
					simulationLogPtr->updateStatus(SimulationStatus::Computed);
				}
			}
			simulationHistory.emplace(std::make_pair(simInfo, simulationLogPtr));
		}
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
			{
				std::lock_guard<std::mutex> lock(promiseMutex);
				bool fullfilled = true;
				while (fullfilled) {
					fullfilled = false;
					for (auto& promisePair : simulationResultPromises) {
						fullfilled = fullfillSimulationResultPromise(promisePair.first);
						if (fullfilled) {
							break;
						}
					}
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	};
	scanThread = std::jthread(scanThreadLambda);
}

void Database::stopSyncLoop() {
	keepSyncing = false;
}

SimulationStatus Database::status(SimulationInfo simInfo) const {
	std::lock_guard<std::recursive_mutex> lock(dbMutex);
	if (!simulationHistory.contains(simInfo)) {
		return SimulationStatus::NonExistent;
	}
	return simulationHistory.at(simInfo)->status();
}

SimulationLogPtr Database::registerSimulation(SimulationInfo simInfo) {
	std::lock_guard<std::recursive_mutex> lock(dbMutex);
	if (SimulationStatus::NonExistent != status(simInfo)) {
		return getSimulationLog(simInfo);
	}
	SimulationLogPtr simulationLogPtr = std::make_shared<SimulationLog>(simInfo);
	auto emplaceResult = simulationHistory.emplace(std::make_pair(simInfo, simulationLogPtr));
	if (!simulationHistory.contains(simInfo)) {
		throw;
	}
	simulationLogPtr->updateStatus(SimulationStatus::Uninitialised);
	addToList(uninitialised, simInfo);
	return simulationLogPtr;
}

MGEA::ErrorCode Database::startSimulation(SimulationInfo simInfo) {
	std::lock_guard<std::recursive_mutex> lock(dbMutex);
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

std::shared_future<MaybeSimulationDataPtr> Database::requestSimulationResult(SimulationInfo simInfo) {
	std::lock_guard<std::mutex> lock(promiseMutex);
	if (!listContains(pendingSimulation, simInfo)) {
		throw(std::logic_error(std::format("SimulationResult for ({}, {}) requested, but it is not pending simulation!", simInfo.generation, simInfo.identifier)));
	}
	if (!simulationResultPromises.contains(simInfo)) {
		simulationResultPromises.emplace(std::make_pair(simInfo, std::promise<MaybeSimulationDataPtr>()));
	}
	return simulationResultPromises.at(simInfo).get_future();
}

MaybeSimulationDataPtr Database::getSimulationResult(SimulationInfo simInfo) {
	std::lock_guard<std::recursive_mutex> lock(dbMutex);
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
	std::lock_guard<std::recursive_mutex> lock(dbMutex);
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

bool Database::fullfillSimulationResultPromise(SimulationInfo simInfo) {
	SimulationLogPtr simulationLogPtr = getSimulationLog(simInfo);
	if (!simulationLogPtr) {
		return false;
	}
	if (simulationLogPtr->outputExists()) {
		simulationResultPromises.at(simInfo).set_value(simulationLogPtr->data());
		return false;
	}
	MGEA::ErrorCode importError = datastore.importOutputFile(simulationLogPtr);
	if (MGEA::ErrorCode::OK != importError) {
		return false;
	}
	if (simulationLogPtr->data()->error) {
		spdlog::warn("Simulation error for {}. Error message was:", simInfo);
		spdlog::warn("{}", *simulationLogPtr->data()->error);
		simulationLogPtr->updateStatus(SimulationStatus::SimulationError);
		removeFromList(pendingSimulation, simInfo);
	} else {
		simulationLogPtr->updateStatus(SimulationStatus::PendingEvaluation);
		moveFromListToList(pendingSimulation, pendingEvaluation, simInfo);
	}
	simulationResultPromises.at(simInfo).set_value(simulationLogPtr->data());
	std::erase_if(simulationResultPromises, [&](auto const& item) { return item.first == simInfo; });
	return true;
}

bool Database::listContains(SimulationInfoList & list, SimulationInfo simInfo) {
	auto listIt = std::find(list.begin(), list.end(), simInfo);
	return listIt != list.end();
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
