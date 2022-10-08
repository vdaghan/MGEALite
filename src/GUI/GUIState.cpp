#include "GUI/GUIState.h"

std::mutex & GUIState::getMutexRef() {
	return updateLock;
}

void GUIState::updateGenealogy(Spec::Genealogy const & genealogy) {
	allGenerationFitness.clear();
	for (auto const & generation : genealogy) {
		allGenerationFitness.push_back({});
		for (auto const & iPtr : generation) {
			if (iPtr->maybePhenotypeProxy.has_value()) {
				allGenerationFitness.back().push_back(iPtr->fitness);
			}
		}
	}
	calculateFitnesses();
}

void GUIState::calculateFitnesses() {
	std::lock_guard<std::mutex> lock(std::mutex updateLock);

	minFitness.clear();
	maxFitness.clear();
	lastFitness.clear();
	minFitnesses.clear();
	maxFitnesses.clear();
	meanFitnesses.clear();

	lastFitness = allGenerationFitness.back();
	for (auto & generationFitness : allGenerationFitness) {
		if (minFitness.size() < generationFitness.size()) {
			minFitness.resize(generationFitness.size(), std::numeric_limits<double>::max());
		}
		if (maxFitness.size() < generationFitness.size()) {
			maxFitness.resize(generationFitness.size(), std::numeric_limits<double>::lowest());
		}
		for (std::size_t i(0); i != generationFitness.size(); ++i) {
			minFitness[i] = std::min(minFitness[i], generationFitness[i]);
			maxFitness[i] = std::max(maxFitness[i], generationFitness[i]);
		}
	}

	for (auto & generation : allGenerationFitness) {
		minFitnesses.push_back(generation.back());
		maxFitnesses.push_back(generation.front());

		std::size_t numFitnesses(generation.size());
		double fitnessTotal = std::accumulate(generation.begin(), generation.end(), 0.0);
		meanFitnesses.push_back(fitnessTotal / static_cast<double>(numFitnesses));
	}
}
