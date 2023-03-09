// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Datastore.h"

#include "spdlog/spdlog.h"
#include "JSON/SimulationDataPtr.h"

#include <algorithm>
#include <system_error>

std::optional<std::size_t> stringToSizeT(std::string s) {
	std::size_t retVal;
	try {
		retVal = std::stoul(s);
	} catch (const std::exception &) {
		return std::nullopt;
	}
	return retVal;
}

Datastore::Datastore(std::filesystem::path path)
	: simId(0),
	m_path(path),
	m_queuePath(path / "queue"),
	m_visualisationPath(path / "visualisation"),
	keepSyncing(false)
{
	std::filesystem::remove_all(m_path);
	std::error_code pathCreateError{};
	bool pathCreated = std::filesystem::create_directories(m_path, pathCreateError);
	std::error_code queuePathCreateError{};
	bool queuePathCreated = std::filesystem::create_directories(m_queuePath, queuePathCreateError);
	std::filesystem::create_directory(m_visualisationPath);
	
	if (not pathCreated) {
		spdlog::error("Data folder creation failed: {}", pathCreateError.message());
	}
	if (not queuePathCreated) {
		spdlog::error("Queue folder creation failed: {}", queuePathCreateError.message());
	}

	startSyncLoop();
}

Datastore::~Datastore() {
	stopSyncLoop();
	syncWithFilesystem();
}

Spec::MaybePhenotype Datastore::simulate(Spec::Genotype genotype) {
	std::size_t id(simId.fetch_add(1));
	auto const simulationInputPath(toInputPath(id));
	auto exportError = MGEA::exportSimulationDataPtrToFile(genotype, simulationInputPath);
	if (DEvA::ErrorCode::OK != exportError) {
		return std::unexpected(exportError);
	}
	{
		std::lock_guard<std::mutex> lock(promiseMutex);
		//simulationResultPromises.emplace(std::make_pair(id, std::promise<Spec::MaybePhenotype>()));
		simulationResultPromises.emplace(id, std::promise<Spec::MaybePhenotype>());
	}
	promiseMutex.lock();
	auto & simulationResultPromise(simulationResultPromises.at(id));
	promiseMutex.unlock();
	auto simulationResultFuture(simulationResultPromise.get_future());
	auto simulationResult(simulationResultFuture.get());
	{
		std::lock_guard<std::mutex> lock(promiseMutex);
		simulationResultPromises.erase(id);
	}
	return simulationResult;
}

std::list<SimulationFile> Datastore::scanFiles() {
	std::list<SimulationFile> simulationFiles;
	for (auto & d : std::filesystem::directory_iterator{m_queuePath}) {
		if (d.path() == m_queuePath) {
			continue;
		}
		std::string const fileNameStem = d.path().stem().string();
		std::string const fileNameExtension = d.path().extension().string().substr(1);

		auto idOpt = stringToSizeT(fileNameStem);
		if (!idOpt) {
			continue;
		}
		simulationFiles.emplace_back(SimulationFile{.id = *idOpt, .type = fileNameExtension});
	}
	return simulationFiles;
}

Progress Datastore::getProgress(std::list<SimulationFile> simulationFiles) {
	Progress progress;
	for (auto & simulationFile : simulationFiles) {
		if ("input" == simulationFile.type) {
			progress.idsWithInputs.emplace_back(simulationFile.id);
		} else if ("output" == simulationFile.type) {
			progress.idsWithOutputs.emplace_back(simulationFile.id);
		}
	}

	// TODO: These should be unnecessary
	unique(progress.idsWithInputs);
	unique(progress.idsWithOutputs);

	std::set_intersection(progress.idsWithInputs.begin(), progress.idsWithInputs.end(),
		progress.idsWithOutputs.begin(), progress.idsWithOutputs.end(),
		std::back_inserter(progress.idsWithInputsAndOutputs));
	unique(progress.idsWithInputsAndOutputs);

	return progress;
}

std::list<std::size_t> Datastore::fulfillPromises(Progress progress) {
	std::lock_guard<std::mutex> lock(promiseMutex);
	std::list<std::size_t> fulfilledPromises;
	for (auto & idWithInputAndOutput : progress.idsWithInputsAndOutputs) {
		auto & promise(simulationResultPromises.at(idWithInputAndOutput));

		auto const simulationOutputPath(toOutputPath(idWithInputAndOutput));
		if (not std::filesystem::exists(simulationOutputPath)) {
			continue;
		}
		auto simulationOutput(MGEA::importSimulationDataPtrFromFile(simulationOutputPath));
		if (not simulationOutput and std::unexpected(DEvA::ErrorCode::InvalidTransform) != simulationOutput) {
			continue;
		}
		simulationResultPromises.at(idWithInputAndOutput).set_value(simulationOutput);
		fulfilledPromises.emplace_back(idWithInputAndOutput);
	}
	return fulfilledPromises;
}

std::list<std::filesystem::path> Datastore::getObsoleteFiles(std::list<std::size_t> toDelete) {
	std::list<std::filesystem::path> obsoleteFiles;
	for (auto & idToDelete : toDelete) {
		obsoleteFiles.emplace_back(toInputPath(idToDelete));
		obsoleteFiles.emplace_back(toOutputPath(idToDelete));
	}
	return obsoleteFiles;
}

void Datastore::updateDeleteQueue(std::list<std::filesystem::path> obsoleteFiles) {
	unique(obsoleteFiles);
	unique(deleteQueue);
	deleteQueue.merge(obsoleteFiles);
	unique(deleteQueue);
}

void Datastore::deleteFiles() {
	std::list<std::filesystem::path> deletedFiles{};
	for (auto & fileToDelete : deleteQueue) {
		if (not std::filesystem::exists(fileToDelete)) {
			deletedFiles.emplace_back(fileToDelete);
			continue;
		}
		bool removed = std::filesystem::remove(fileToDelete);
		if (removed) {
			deletedFiles.emplace_back(fileToDelete);
		}
	}
	for (auto & deletedFile : deletedFiles) {
		deleteQueue.remove(deletedFile);
	}
}

void Datastore::syncWithFilesystem() {
	auto && simulationFiles(scanFiles());
	auto && progress(getProgress(simulationFiles));
	auto && fulfilledPromises(fulfillPromises(progress));
	auto && newObsoleteFiles(getObsoleteFiles(fulfilledPromises));
	updateDeleteQueue(newObsoleteFiles);
	deleteFiles();
}

void Datastore::startSyncLoop() {
	keepSyncing = true;
	auto scanThreadLambda = [this]() {
		while (keepSyncing) {
			syncWithFilesystem();
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	};
	syncThread = std::jthread(scanThreadLambda);
}

void Datastore::stopSyncLoop() {
	keepSyncing = false;
}

std::filesystem::path Datastore::toInputPath(std::size_t id) {
	return std::filesystem::path(m_queuePath / (std::to_string(id) + ".input"));
}

std::filesystem::path Datastore::toOutputPath(std::size_t id) {
	return std::filesystem::path(m_queuePath / (std::to_string(id) + ".output"));
}

std::filesystem::path Datastore::toGenerationPath(DEvA::IndividualIdentifier simInfo) {
	return std::filesystem::path(m_path / std::to_string(simInfo.generation));
}


//MGEA::ErrorCode Datastore::saveVisualisationTarget(DEvA::IndividualIdentifier simInfo) {
//	auto const combinedFilePath = toCombinedPath(simInfo);
//	std::filesystem::path const visualisationFilePath(m_visualisationPath / "visualise.json");
//	if (!std::filesystem::exists(combinedFilePath)) [[unlikely]] {
//		return MGEA::ErrorCode::FileNotFound;
//	}
//	std::error_code copyError;
//	std::filesystem::copy(combinedFilePath, visualisationFilePath, std::filesystem::copy_options::overwrite_existing, copyError);
//	return MGEA::ErrorCode::OK;
//}