#include "Database.h"

#include <algorithm>
#include <iostream>
#include <ranges>

Database::Database(std::string pathName) : path(pathName), scan(true) {
	if (!std::filesystem::exists(path)) {
		std::filesystem::create_directory(path);
	}
	rescanThread = std::jthread(&Database::rescan, this);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

Database::~Database() {
	scan = false;
}

void Database::rescan() {
	while (scan) {
		std::lock_guard dbLock(dbMutex);
		std::cout << "Scanning folders..." << std::endl;
		std::string pathFolder = path.filename().string();
		std::cout << "Path folder is " << pathFolder << std::endl;
		for (auto & d : std::filesystem::directory_iterator{path}) {
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
				auto simLogPtr = getSimulation(generation, individual);

			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

SimulationLogPtr Database::getSimulation(std::size_t gen, std::size_t id) {
	std::lock_guard dbLock(dbMutex);
	if (!simulations.contains(gen)) {
		return nullptr;
	}
	if (!simulations.at(gen).contains(id)) {
		return nullptr;
	}
	return simulations.at(gen).at(id);
}

SimulationLogPtr Database::createSimulation(std::size_t gen, std::size_t id) {
	std::lock_guard dbLock(dbMutex);
	if (!simulations.contains(gen)) {
		simulations.emplace(std::make_pair(gen, SimulationLogPtrMap()));
	}
	if (!simulations.at(gen).contains(id)) {
		SimulationLog * simulationLogRawPtr = new SimulationLog(gen, id, path);
		simulations.at(gen).emplace(id, simulationLogRawPtr);
	}
	return simulations.at(gen).at(id);
}

UpdatedSimulationList Database::getUpdatedSimulations() {
	std::lock_guard dbLock(dbMutex);
	UpdatedSimulationList retVal(std::move(updatedSimulations));
	updatedSimulations.clear();
	return retVal;
}
