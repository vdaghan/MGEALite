#include "Datastore.h"

#include "spdlog/spdlog.h"

#include <algorithm>

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
	: m_path(path),
	m_queuePath(path / "queue"),
	m_visualisationPath(path / "visualisation")
{
	std::filesystem::create_directory(m_path);
	std::filesystem::create_directory(m_queuePath);
	std::filesystem::create_directory(m_visualisationPath);

	std::size_t gen(0);
	std::size_t id(0);
	while (true) {
		std::filesystem::path genPath{ m_path / std::to_string(gen) };
		if (!std::filesystem::exists(genPath)) {
			break;
		}
		while (true) {
			std::filesystem::path individualPath{ genPath / (std::to_string(id) + ".json") };
			if (!std::filesystem::exists(individualPath)) {
				break;
			}
			addToHistory(SimulationInfo{.generation=gen, .identifier=id});
			++id;
		}
		++gen;
	}
}

Datastore::~Datastore() {
	syncWithFilesystem();
}

void Datastore::syncWithFilesystem() {
	deleteAllArtifacts();
	Progress progress = getProgress();

	std::set_intersection(progress.idsWithInputs.begin(), progress.idsWithInputs.end(),
		progress.idsWithOutputs.begin(), progress.idsWithOutputs.end(),
		std::back_inserter(progress.idsWithInputsAndOutputs));
	for (auto & idWithInputAndOutput : progress.idsWithInputsAndOutputs) {
		simulationQueue.remove(idWithInputAndOutput);
		evaluationQueue.push_back(idWithInputAndOutput);
	}

	std::list<std::size_t> tmp;
	std::set_union(evaluationQueue.begin(), evaluationQueue.end(),
		progress.idsWithOutputs.begin(), progress.idsWithOutputs.end(),
		std::back_inserter(tmp));
	std::list<std::size_t> updatedEvaluationQueue;
	std::set_difference(tmp.begin(), tmp.end(),
		deleteQueue.begin(), deleteQueue.end(),
		std::back_inserter(updatedEvaluationQueue));
	evaluationQueue = updatedEvaluationQueue;
}

MGEA::ErrorCode Datastore::exportInputFile(SimulationLogPtr simLogPtr) {
	if (!simLogPtr->inputExists()) {
		return MGEA::ErrorCode::NoInput;
	}
	auto exportError = exportSimulationData(simLogPtr->data(), toInputPath(simLogPtr->info().identifier));
	return exportError;
}

MGEA::ErrorCode Datastore::importOutputFile(SimulationLogPtr simLogPtr) {
	if (simLogPtr->outputExists()) {
		return MGEA::ErrorCode::OK;
	}
	auto maybeOutputData = importSimulationData(toOutputPath(simLogPtr->info().identifier));
	if (!maybeOutputData) {
		return maybeOutputData.error();
	}
	updateSimulationDataPtr({.source = maybeOutputData.value(), .target = simLogPtr->data()});
	return MGEA::ErrorCode::OK;
}

MGEA::ErrorCode Datastore::setFitnessAndCombineFiles(SimulationLogPtr slptr, double fitness) {
	auto const simInfo = slptr->info();
	auto const inputFile = toInputPath(simInfo.identifier);
	auto const outputFile = toOutputPath(simInfo.identifier);
	auto const combinedFile = toCombinedPath(simInfo);
	if (std::filesystem::exists(combinedFile)) [[unlikely]] {
		return MGEA::ErrorCode::OK;
	}
	if (!std::filesystem::exists(inputFile) or !std::filesystem::exists(outputFile)) [[unlikely]] {
		return MGEA::ErrorCode::FileNotFound;
	}
	MaybeSimulationDataPtr maybeInputDataPtr = importSimulationData(inputFile);
	if (!maybeInputDataPtr) {
		return maybeInputDataPtr.error();
	}
	MaybeSimulationDataPtr maybeOutputDataPtr = importSimulationData(outputFile);
	if (!maybeOutputDataPtr) {
		return maybeOutputDataPtr.error();
	}
	auto const generationFolder = toGenerationPath(simInfo);
	if (!std::filesystem::exists(generationFolder)) [[unlikely]] {
		std::filesystem::create_directory(generationFolder);
	}
	//SimulationDataPtr combinedDataPtr = SimulationDataPtr(new SimulationData());
	SimulationDataPtr combinedDataPtr = std::make_shared<SimulationData>();
	updateSimulationDataPtr({.source = maybeInputDataPtr.value(), .target = combinedDataPtr});
	updateSimulationDataPtr({.source = maybeOutputDataPtr.value(), .target = combinedDataPtr});
	combinedDataPtr->fitness = fitness;
	MGEA::ErrorCode exportError = exportSimulationData(combinedDataPtr, combinedFile);
	if (MGEA::ErrorCode::OK != exportError) {
		return exportError;
	}
	addToHistory(simInfo);
	deleteQueue.push_back(simInfo.identifier);
	return MGEA::ErrorCode::OK;
}

MaybeSimulationDataPtr Datastore::importCombinedFile(SimulationInfo simInfo) {
	std::filesystem::path const file = toCombinedPath(simInfo);
	if (!std::filesystem::exists(file)) {
		return std::unexpected(MGEA::ErrorCode::FileNotFound);
	}
	return importSimulationData(file);
}

bool Datastore::existsInHistory(SimulationInfo simInfo) {
	auto it = std::find(m_history.begin(), m_history.end(), simInfo);
	return it != m_history.end();
}

MGEA::ErrorCode Datastore::saveVisualisationTarget(SimulationInfo simInfo) {
	auto const combinedFilePath = toCombinedPath(simInfo);
	std::filesystem::path const visualisationFilePath(m_visualisationPath / "visualise.json");
	if (!std::filesystem::exists(combinedFilePath)) [[unlikely]] {
		return MGEA::ErrorCode::FileNotFound;
	}
	std::error_code copyError;
	std::filesystem::copy(combinedFilePath, visualisationFilePath, std::filesystem::copy_options::overwrite_existing, copyError);
	return MGEA::ErrorCode::OK;
}

Progress Datastore::getProgress() {
	Progress progress;
	auto & idsWithInputs = progress.idsWithInputs;
	auto & idsWithOutputs = progress.idsWithOutputs;
	for (auto & d : std::filesystem::directory_iterator{m_queuePath}) {
		if (d.path() == m_queuePath) {
			continue;
		}
		std::string const fileNameStem = d.path().stem().string();
		std::string const fileNameExtension = d.path().extension().string();

		auto idOpt = stringToSizeT(fileNameStem);
		if (!idOpt) {
			continue;
		}
		if ("input" == fileNameExtension) {
			idsWithInputs.push_back(*idOpt);
		} else if ("output" == fileNameExtension) {
			idsWithOutputs.push_back(*idOpt);
		}
	}
	for (auto & d : std::filesystem::directory_iterator{m_path}) {
		if (d.path() == m_path) {
			continue;
		}
		auto genOpt = stringToSizeT(d.path().filename().string());
		if (!genOpt) {
			continue;
		}
		std::filesystem::path generationPath(m_path / std::to_string(*genOpt));
		for (auto & dd : std::filesystem::directory_iterator{generationPath}) {
			if (std::filesystem::is_directory(dd.path())) {
				continue;
			}
			std::string const fileNameStem = d.path().stem().string();
			std::string const fileNameExtension = d.path().extension().string();
			if ("json" != fileNameExtension) {
				continue;
			}
			auto idOpt = stringToSizeT(fileNameStem);
			if (!idOpt) {
				continue;
			}
			addToHistory(SimulationInfo{.generation = *genOpt, .identifier = *idOpt});
		}
	}
	return progress;
}

bool Datastore::deleteArtifacts(std::size_t id) {
	auto inputFile = toInputPath(id);
	auto outputFile = toOutputPath(id);
	if (std::filesystem::exists(inputFile)) [[likely]] {
		bool removed = std::filesystem::remove(inputFile);
		if (!removed) {
			return false;
		}
	}
	if (std::filesystem::exists(outputFile)) [[likely]] {
		bool removed = std::filesystem::remove(outputFile);
		if (!removed) {
			return false;
		}
	}
	deleteQueue.remove(id);
	return true;
}

void Datastore::deleteAllArtifacts() {
	std::list<std::size_t> tmp = deleteQueue;
	for (auto & toDelete : tmp) {
		deleteArtifacts(toDelete);
	}
}

void Datastore::addToHistory(SimulationInfo simInfo) {
	if (existsInHistory(simInfo)) {
		return;
	}
	m_history.push_back(simInfo);
	m_history.sort();
}

std::filesystem::path Datastore::toInputPath(std::size_t id) {
	return std::filesystem::path(m_queuePath / (std::to_string(id) + ".input"));
}

std::filesystem::path Datastore::toOutputPath(std::size_t id) {
	return std::filesystem::path(m_queuePath / (std::to_string(id) + ".output"));
}

std::filesystem::path Datastore::toCombinedPath(SimulationInfo simInfo) {
	return std::filesystem::path(m_path / std::to_string(simInfo.generation) / (std::to_string(simInfo.identifier) + ".json"));
}

std::filesystem::path Datastore::toGenerationPath(SimulationInfo simInfo) {
	return std::filesystem::path(m_path / std::to_string(simInfo.generation));
}
