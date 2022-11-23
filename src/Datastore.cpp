// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

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
			addToHistory(DEvA::IndividualIdentifier{.generation=gen, .identifier=id});
			++id;
		}
		++gen;
	}
}

Datastore::~Datastore() {
	syncWithFilesystem();
}

void Datastore::syncWithFilesystem() {
	std::lock_guard<std::mutex> lock(queueMutex);
	deleteAllArtifacts();
	Progress progress = getProgress();

	auto sortAndRemoveLambda = [](std::list<std::size_t>& list) {
		std::stable_sort(list.begin(), list.end());
		auto last = std::unique(list.begin(), list.end());
		list.erase(last, list.end());
	};
	sortAndRemoveLambda(progress.idsWithInputs);
	sortAndRemoveLambda(progress.idsWithOutputs);
	std::set_intersection(progress.idsWithInputs.begin(), progress.idsWithInputs.end(),
		progress.idsWithOutputs.begin(), progress.idsWithOutputs.end(),
		std::back_inserter(progress.idsWithInputsAndOutputs));
	sortAndRemoveLambda(progress.idsWithInputsAndOutputs);
	for (auto & idWithInputAndOutput : progress.idsWithInputsAndOutputs) {
		simulationQueue.remove(idWithInputAndOutput);
		evaluationQueue.push_back(idWithInputAndOutput);
	}

	std::list<std::size_t> tmp;
	sortAndRemoveLambda(evaluationQueue);
	std::set_union(evaluationQueue.begin(), evaluationQueue.end(),
		progress.idsWithOutputs.begin(), progress.idsWithOutputs.end(),
		std::back_inserter(tmp));
	std::list<std::size_t> updatedEvaluationQueue;
	sortAndRemoveLambda(deleteQueue);
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

MGEA::ErrorCode Datastore::combineFilesWithMetrics(SimulationLogPtr slptr, Spec::SMetricMap metricMap) {
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
	//combinedDataPtr->metrics = metrics;
	MGEA::ErrorCode exportError = exportSimulationData(combinedDataPtr, combinedFile);
	if (MGEA::ErrorCode::OK != exportError) {
		return exportError;
	}
	std::lock_guard<std::mutex> lock(queueMutex);
	addToHistory(simInfo);
	deleteQueue.push_back(simInfo.identifier);
	return MGEA::ErrorCode::OK;
}

MaybeSimulationDataPtr Datastore::importCombinedFile(DEvA::IndividualIdentifier simInfo) {
	std::filesystem::path const file = toCombinedPath(simInfo);
	if (!std::filesystem::exists(file)) {
		return std::unexpected(MGEA::ErrorCode::FileNotFound);
	}
	return importSimulationData(file);
}

bool Datastore::existsInHistory(DEvA::IndividualIdentifier simInfo) {
	std::lock_guard<std::recursive_mutex> lock(historyMutex);
	auto & gen(simInfo.generation);
	if (m_history.size() <= gen) {
		return false;
	}
	auto & generationHistory(m_history.at(gen));
	auto it = std::find(generationHistory.begin(), generationHistory.end(), simInfo);
	return it != generationHistory.end();
}

MGEA::ErrorCode Datastore::saveVisualisationTarget(DEvA::IndividualIdentifier simInfo) {
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
			addToHistory(DEvA::IndividualIdentifier{.generation = *genOpt, .identifier = *idOpt});
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

void Datastore::addToHistory(DEvA::IndividualIdentifier simInfo) {
	std::lock_guard<std::recursive_mutex> lock(historyMutex);
	if (existsInHistory(simInfo)) {
		return;
	}
	if (m_history.size() <= simInfo.generation) {
		m_history.resize(simInfo.generation + 1);
	}
	m_history.at(simInfo.generation).push_back(simInfo);
	m_history.at(simInfo.generation).sort();
}

std::filesystem::path Datastore::toInputPath(std::size_t id) {
	return std::filesystem::path(m_queuePath / (std::to_string(id) + ".input"));
}

std::filesystem::path Datastore::toOutputPath(std::size_t id) {
	return std::filesystem::path(m_queuePath / (std::to_string(id) + ".output"));
}

std::filesystem::path Datastore::toCombinedPath(DEvA::IndividualIdentifier simInfo) {
	return std::filesystem::path(m_path / std::to_string(simInfo.generation) / (std::to_string(simInfo.identifier) + ".json"));
}

std::filesystem::path Datastore::toGenerationPath(DEvA::IndividualIdentifier simInfo) {
	return std::filesystem::path(m_path / std::to_string(simInfo.generation));
}
