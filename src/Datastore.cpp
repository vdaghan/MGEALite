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

bool Datastore::combineInputOutputAndFitness(SimulationInfo simInfo, double fitness) {
	auto inputFile = toInputPath(simInfo.identifier);
	auto outputFile = toOutputPath(simInfo.identifier);
	auto combinedFile = toCombinedPath(simInfo);
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
	addToHistory(simInfo);
	deleteQueue.push_back(simInfo.identifier);
	return true;
}

bool Datastore::deleteUncombined(std::size_t id) {
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

bool Datastore::existsInHistory(SimulationInfo simInfo) {
	if (m_history.size() < simInfo.generation) {
		return false;
	}
	auto & historyGeneration = m_history[simInfo.generation];
	auto it = std::find(historyGeneration.begin(), historyGeneration.end(), simInfo);
	return it == historyGeneration.end();
	return false;
}

void Datastore::addToHistory(SimulationInfo simInfo) {
	if (existsInHistory(simInfo)) {
		return;
	}
	if (m_history.size() < simInfo.generation) {
		m_history.resize(simInfo.generation + 1);
	}
	m_history[simInfo.generation].push_back(simInfo);
	m_history[simInfo.generation].sort();
}

std::filesystem::path Datastore::toInputPath(std::size_t id) {
	return std::filesystem::path(m_queuePath / std::to_string(id) / ".input");
}

std::filesystem::path Datastore::toOutputPath(std::size_t id) {
	return std::filesystem::path(m_queuePath / std::to_string(id) / ".output");
}

std::filesystem::path Datastore::toCombinedPath(SimulationInfo simInfo) {
	return std::filesystem::path(m_path / std::to_string(simInfo.generation) / std::to_string(simInfo.identifier) / ".json");
}

void Datastore::sync() {
	for (auto & toDelete : deleteQueue) {
		bool deletionSuccessful = deleteUncombined(toDelete);
		if (!deletionSuccessful) [[unlikely]] {
			continue;
		}
		deleteQueue.remove(toDelete);
	}
	std::list<std::size_t> idsWithInputs;
	std::list<std::size_t> idsWithOutputs;
	std::list<std::size_t> idsCombined;
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
			idsCombined.push_back(*idOpt);
			addToHistory(SimulationInfo{.generation = *genOpt, .identifier = *idOpt});
		}
	}
	{
		std::lock_guard<std::mutex> dsLock(dsMutex);
		std::list<std::size_t> idsWithInputsAndOutputs;
		std::set_intersection(idsWithInputs.begin(), idsWithInputs.end(),
			idsWithOutputs.begin(), idsWithOutputs.end(),
			std::back_inserter(idsWithInputsAndOutputs));
		for (auto & idWithInputAndOutput : idsWithInputsAndOutputs) {
			simulationQueue.remove(idWithInputAndOutput);
			evaluationQueue.push_back(idWithInputAndOutput);
		}
	}
	{
		std::lock_guard<std::mutex> dsLock(dsMutex);
		std::list<std::size_t> tmp;
		std::set_union(evaluationQueue.begin(), evaluationQueue.end(),
			idsWithOutputs.begin(), idsWithOutputs.end(),
			std::back_inserter(tmp));
		std::list<std::size_t> updatedEvaluationQueue;
		std::set_difference(tmp.begin(), tmp.end(),
			deleteQueue.begin(), deleteQueue.end(),
			std::back_inserter(updatedEvaluationQueue));
		evaluationQueue = updatedEvaluationQueue;
	}
}
