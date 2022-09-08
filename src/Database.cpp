#include "Database.h"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <iostream>
#include <ranges>

Database::Database(std::string pathName) : datastore(pathName), m_nextId(0) {
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
	auto emplaceResult = simulationHistory.emplace(std::make_pair(simInfo, simulationLogPtr));
	if (!simulationHistory.contains(simInfo)) {
		return nullptr;
	}
	++m_nextId;
	addToList(uninitialised, simInfo);
	return simulationLogPtr->data();
}

bool Database::startSimulation(SimulationInfo simInfo) {
	if (!simulationHistory.contains(simInfo)) {
		return false;
	}
	SimulationLogPtr simulationLogPtr = simulationHistory.at(simInfo);
	moveFromListToList(uninitialised, pendingSimulation, simInfo);
	return datastore.exportInputFile(simulationLogPtr);
}

SimulationDataPtr Database::getSimulationResult(SimulationInfo simInfo) {
	if (!simulationHistory.contains(simInfo)) {
		return nullptr;
	}
	SimulationLogPtr simulationLogPtr = simulationHistory.at(simInfo);
	if (!simulationLogPtr->outputExists()) {
		return nullptr;
	}

	moveFromListToList(pendingSimulation, pendingEvaluation, simInfo);
	return simulationLogPtr->data();
}

bool Database::setSimulationFitness(SimulationInfo simInfo, double fitness) {
	SimulationLogPtr simulationLogPtr = getSimulationLog(simInfo);
	if (!simulationLogPtr) {
		return false;
	}
	bool setSuccessful = datastore.setFitnessAndCombineFiles(simulationLogPtr, fitness);
	if (!setSuccessful) {
		return false;
	}
	moveFromListToList(pendingEvaluation, computed, simInfo);
	return true;
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
