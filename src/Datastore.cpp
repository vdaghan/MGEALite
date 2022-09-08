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
	m_queuePath(path / "queue")
{
	std::filesystem::create_directory(m_path);
	std::filesystem::create_directory(m_queuePath);
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

bool Datastore::exportInputFile(SimulationLogPtr simLogPtr) {
	if (!simLogPtr->inputExists()) {
		return false;
	}
	bool exportSuccessful = exportSimulationData(simLogPtr->data(), toInputPath(simLogPtr->info().identifier));
	if (!exportSuccessful) {
		return false;
	}
	simLogPtr->updateStatus(SimulationStatus::PendingSimulation);
	return true;
}

bool Datastore::importOutputFile(SimulationLogPtr simLogPtr) {
	if (simLogPtr->outputExists()) {
		return true;
	}
	SimulationDataPtr outputData = importSimulationData(toOutputPath(simLogPtr->info().identifier));
	if (!outputData) {
		return false;
	}
	simLogPtr->data()->outputs = outputData->outputs;
	simLogPtr->updateStatus(SimulationStatus::PendingEvaluation);
	return true;
}

bool Datastore::setFitnessAndCombineFiles(SimulationLogPtr slptr, double fitness) {
	auto const simInfo = slptr->info();
	auto const inputFile = toInputPath(simInfo.identifier);
	auto const outputFile = toOutputPath(simInfo.identifier);
	auto const combinedFile = toCombinedPath(simInfo);
	if (std::filesystem::exists(combinedFile)) [[unlikely]] {
		return true;
	}
	if (!std::filesystem::exists(inputFile) or !std::filesystem::exists(outputFile)) [[unlikely]] {
		return false;
	}
	SimulationDataPtr inputDataPtr = importSimulationData(inputFile);
	SimulationDataPtr outputDataPtr = importSimulationData(outputFile);
	if (!inputDataPtr or !outputDataPtr) {
		return false;
	}
	SimulationDataPtr combinedDataPtr = SimulationDataPtr(new SimulationData());
	combinedDataPtr->time = inputDataPtr->time;
	combinedDataPtr->params = inputDataPtr->params;
	combinedDataPtr->torque = inputDataPtr->torque;
	combinedDataPtr->outputs = outputDataPtr->outputs;
	combinedDataPtr->fitness = fitness;
	bool exportSuccessful = exportSimulationData(combinedDataPtr, combinedFile);
	if (!exportSuccessful) {
		return false;
	}
	slptr->updateStatus(SimulationStatus::Computed);
	addToHistory(simInfo);
	deleteQueue.push_back(simInfo.identifier);
	return true;
}

bool Datastore::existsInHistory(SimulationInfo simInfo) {
	auto it = std::find(m_history.begin(), m_history.end(), simInfo);
	return it == m_history.end();
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
	for (auto & toDelete : deleteQueue) {
		bool deletionSuccessful = deleteArtifacts(toDelete);
		if (!deletionSuccessful) [[unlikely]] {
			continue;
		}
		deleteQueue.remove(toDelete);
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
	return std::filesystem::path(m_path / std::to_string(simInfo.generation) / std::to_string(simInfo.identifier) / ".json");
}
