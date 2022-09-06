#include "Database.h"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <iostream>
#include <ranges>

Database::Database(std::string pathName)
	: path(pathName),
	maxSimulationId(std::nullopt),
	scan(true)
{
	if (!std::filesystem::exists(path)) {
		std::filesystem::create_directory(path);
	}
	rescan(true);
	rescanThread = std::jthread(&Database::rescan, this, false);
}

Database::~Database() {
	scan = false;
}

void Database::rescan(bool once) {
	while (scan || once) {
		spdlog::info("Scanning folders...");
		std::string pathFolder = path.filename().string();
		spdlog::info("Path folder is \"{}\"", pathFolder);
		for (auto & d : std::filesystem::directory_iterator{path}) {
			std::lock_guard<std::mutex> dbLock(dbMutex);
			if (!std::filesystem::is_directory(d.path())) {
				continue;
			}
			std::string const folderName = d.path().filename().string();
			if (folderName == pathFolder) {
				continue;
			}
			std::size_t generation;
			try {
				generation = std::stoul(folderName);
				if (folderName != std::to_string(generation)) {
					continue;
				}
			} catch (const std::exception &) {
				continue;
			}
			std::filesystem::path generationPath(path / std::to_string(generation));
			for (auto & d : std::filesystem::directory_iterator{generationPath}) {
				if (std::filesystem::is_directory(d.path())) {
					continue;
				}
				std::string const fileNameStem = d.path().stem().string();
				std::size_t individual;
				try {
					individual = std::stoul(fileNameStem);
				} catch (const std::exception &) {
					continue;
				}
				updateMaxSimulationId(individual);

				//std::string const fileNameExtension = d.path().extension().string();
				SimulationInfo simulationInfo{.generation = generation, .identifier = individual};
				auto simLogPtr = getSimulation(simulationInfo);
			}
		}

		if (once) [[unlikely]] {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

std::size_t Database::createNewGeneration() {
	std::size_t retVal = simulations.size();
	simulations.push_back(SimulationLogPtrMap());
	return retVal;
}

SimulationLogPtr Database::getSimulation(SimulationInfo simInfo) {
	//std::lock_guard<std::mutex> dbLock(dbMutex); // This results in recursive lock. Make sure we don't need this.
	if (!generationExists(simInfo.generation)) {
		return nullptr;
	}
	if (!simulations.at(simInfo.generation).contains(simInfo.identifier)) {
		return nullptr;
	}
	return simulations.at(simInfo.generation).at(simInfo.identifier);
}

SimulationLogPtr Database::createSimulationInThisGeneration() {
	auto currentGenerationOpt = getCurrentGeneration();
	if (!currentGenerationOpt) {
		return {};
	}
	std::size_t currentGeneration = *currentGenerationOpt;
	std::size_t simulationId;
	if (maxSimulationId) [[likely]] {
		simulationId = *maxSimulationId + 1;
	} else [[unlikely]] {
		simulationId = 0;
	}
	return createSimulation(SimulationInfo{.generation = currentGeneration, .identifier = simulationId});
}

UpdatedSimulationList Database::getUpdatedSimulations() {
	std::lock_guard<std::mutex> dbLock(dbMutex);
	UpdatedSimulationList retVal(std::move(updatedSimulations));
	updatedSimulations.clear();
	return retVal;
}

bool Database::generationExists(std::size_t generation) const {
	return simulations.size() > generation;
}

std::optional<std::size_t> Database::getCurrentGeneration() const {
	if (simulations.empty()) {
		return std::nullopt;
	}
	return simulations.size() - 1;
}

std::size_t Database::getNextGeneration() const {
	return simulations.size();
}

SimulationLogPtr Database::createSimulation(SimulationInfo simInfo) {
	spdlog::info("Creating simulation ({}, {})", simInfo.generation, simInfo.identifier);
	std::lock_guard<std::mutex> dbLock(dbMutex);
	if (!generationExists(simInfo.generation)) {
		if (getNextGeneration() != simInfo.generation) {
			return {};
		}
		createNewGeneration();
	}
	if (!simulations.at(simInfo.generation).contains(simInfo.identifier)) {
		SimulationLog * simulationLogRawPtr = new SimulationLog(simInfo, path);
		simulations.at(simInfo.generation).emplace(simInfo.identifier, simulationLogRawPtr);
		updateMaxSimulationId(simInfo.identifier);
	}
	return simulations.at(simInfo.generation).at(simInfo.identifier);
}

void Database::updateMaxSimulationId(std::size_t id) {
	if (!maxSimulationId) [[unlikely]] {
		maxSimulationId = id;
	} else if (id > *maxSimulationId) {
		maxSimulationId = id;
	}
}
