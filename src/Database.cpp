#include "Database.h"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <iostream>
#include <ranges>

Database::Database(std::string pathName) : path(pathName), scan(true) {
	if (!std::filesystem::exists(path)) {
		std::filesystem::create_directory(path);
	}
	rescanThread = std::jthread(&Database::rescan, this);
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

Database::~Database() {
	scan = false;
}

void Database::rescan() {
	while (scan) {
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

				std::string const fileNameExtension = d.path().extension().string();
				SimulationInfo simulationInfo{.generation = generation, .identifier = individual};
				auto simLogPtr = getSimulation(simulationInfo);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

SimulationLogPtr Database::getSimulation(SimulationInfo simInfo) {
	//std::lock_guard<std::mutex> dbLock(dbMutex); // This results in recursive lock. Make sure we don't need this.
	if (!simulations.contains(simInfo.generation)) {
		return nullptr;
	}
	if (!simulations.at(simInfo.generation).contains(simInfo.identifier)) {
		return nullptr;
	}
	return simulations.at(simInfo.generation).at(simInfo.identifier);
}

SimulationLogPtr Database::createSimulation(SimulationInfo simInfo) {
	spdlog::info("Creating simulation ({}, {})", simInfo.generation, simInfo.identifier);
	std::lock_guard<std::mutex> dbLock(dbMutex);
	if (!simulations.contains(simInfo.generation)) {
		simulations.emplace(std::make_pair(simInfo.generation, SimulationLogPtrMap()));
	}
	if (!simulations.at(simInfo.generation).contains(simInfo.identifier)) {
		SimulationLog * simulationLogRawPtr = new SimulationLog(simInfo.generation, simInfo.identifier, path);
		simulations.at(simInfo.generation).emplace(simInfo.identifier, simulationLogRawPtr);
	}
	return simulations.at(simInfo.generation).at(simInfo.identifier);
}

UpdatedSimulationList Database::getUpdatedSimulations() {
	std::lock_guard<std::mutex> dbLock(dbMutex);
	UpdatedSimulationList retVal(std::move(updatedSimulations));
	updatedSimulations.clear();
	return retVal;
}
