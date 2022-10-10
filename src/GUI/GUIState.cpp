#include "GUI/GUIState.h"

GUIState::GUIState() {
	genealogyProgressData.currentGeneration = 0;
	genealogyProgressData.numberOfGenerations = 0;
}

void GUIState::updateMotionGenerationState(std::size_t generation, MotionGenerationState const & mGS) {
	std::lock_guard<std::mutex> lock(updateMutex);
	if (motionGenerationStates.size() < generation + 1) {
		motionGenerationStates.resize(generation + 1);
	}
	motionGenerationStates.at(generation).updateWith(mGS);
	StateComponentChanged stateComponentChanged = motionGenerationStates.at(generation).changed();
	if (stateComponentChanged & StateComponentChanged_EpochProgress) {
		updateGenerationProgressPlotDatum(generation);
	}
	if (stateComponentChanged & StateComponentChanged_EAProgress) {
		updateGenealogyProgressDatum();
	}
	if (stateComponentChanged & StateComponentChanged_FitnessStatus) {
		updateGenerationFitnessPlotDatum(generation);
		updateGenealogyFitnessPlotDatum();
	}
}

std::size_t GUIState::generations() {
	std::lock_guard<std::mutex> lock(updateMutex);
	return motionGenerationStates.size();
}

std::optional<GenerationProgressPlotDatum> GUIState::generationProgressPlotDatum(std::size_t generation) const {
	std::lock_guard<std::mutex> lock(updateMutex);
	if (generationProgressPlotData.size() >= generation + 1) [[likely]] {
		return generationProgressPlotData.at(generation);
	}
	return std::nullopt;
}

std::optional<GenealogyProgressDatum> GUIState::genealogyProgressDatum() const {
	std::lock_guard<std::mutex> lock(updateMutex);
	return genealogyProgressData;
}

std::optional<GenerationFitnessPlotDatum> GUIState::generationFitnessPlotDatum(std::size_t generation) const {
	std::lock_guard<std::mutex> lock(updateMutex);
	if (generationFitnessPlotData.size() >= generation + 1) [[likely]] {
		return generationFitnessPlotData.at(generation);
	}
	return std::nullopt;
}

std::optional<GenealogyFitnessPlotDatum> GUIState::genealogyFitnessPlotDatum(std::size_t generation) const {
	std::lock_guard<std::mutex> lock(updateMutex);
	if (0 == genealogyFitnessPlotData.numberOfGenerations or 0 == genealogyFitnessPlotData.numberOfIndividuals) {
		return std::nullopt;
	}
	return genealogyFitnessPlotData;
}

void GUIState::updateGenerationProgressPlotDatum(std::size_t generation) {
	MotionGenerationState & mGS = motionGenerationStates.at(generation);
	auto const & eP = mGS.epochProgress();
	GenerationProgressPlotDatum gPPD{
		.failed = eP.failed,
		.evaluated = eP.evaluated,
		.total = eP.total
	};
	if (generationProgressPlotData.size() < generation + 1) {
		generationProgressPlotData.resize(generation + 1);
	}
	generationProgressPlotData.at(generation) = gPPD;
}

void GUIState::updateGenealogyProgressDatum() {
	MotionGenerationState & mGS = motionGenerationStates.back();
	auto const & eP = mGS.eaProgress();
	GenealogyProgressDatum gPD{
		.currentGeneration = eP.currentGeneration,
		.numberOfGenerations = eP.numberOfGenerations
	};
	genealogyProgressData = gPD;
}

void GUIState::updateGenerationFitnessPlotDatum(std::size_t generation) {
	MotionGenerationState & mGS = motionGenerationStates.at(generation);
	auto const & fS = mGS.fitnessStatus();
	Spec::Fitness minimum = *std::min_element(fS.fitnesses.cbegin(), fS.fitnesses.cend());
	Spec::Fitness maximum = *std::max_element(fS.fitnesses.cbegin(), fS.fitnesses.cend());
	std::size_t count = fS.fitnesses.size();
	Spec::Fitness total = std::accumulate(fS.fitnesses.cbegin(), fS.fitnesses.cend(), 0.0);
	Spec::Fitness mean = (0 == count) ? (0.0) : (total / double(count));
	GenerationFitnessPlotDatum gFPD{
		.fitnesses = fS.fitnesses,
		.minimum = minimum,
		.maximum = maximum,
		.mean = mean,
		.count = count
	};
	if (generationFitnessPlotData.size() < generation + 1) {
		generationFitnessPlotData.resize(generation + 1);
	}
	generationFitnessPlotData.at(generation) = gFPD;
}

void GUIState::updateGenealogyFitnessPlotDatum() {
	std::vector<Spec::Fitness> minimumOfGenerations;
	std::vector<Spec::Fitness> maximumOfGenerations;
	std::vector<Spec::Fitness> meanOfGenerations;
	std::size_t numberOfGenerations(0);
	std::vector<Spec::Fitness> minimumOfIndividuals;
	std::vector<Spec::Fitness> maximumOfIndividuals;
	std::vector<Spec::Fitness> meanOfIndividuals;
	std::vector<Spec::Fitness> totalOfIndividuals;
	std::vector<std::size_t> countOfIndividuals;
	std::size_t numberOfIndividuals(0);

	auto maybeResizeLambda = [](std::vector<Spec::Fitness> & data, std::size_t newSize, Spec::Fitness value) {
		if (data.size() < newSize) {
			data.resize(newSize, value);
		}
	};
	for (auto & gFPD : generationFitnessPlotData) {
		minimumOfGenerations.push_back(gFPD.minimum);
		maximumOfGenerations.push_back(gFPD.maximum);
		meanOfGenerations.push_back(gFPD.mean);
		++numberOfGenerations;

		auto individuals = gFPD.count;
		auto & fitnesses = gFPD.fitnesses;
		numberOfIndividuals = std::max(numberOfIndividuals, individuals);
		minimumOfIndividuals.resize(numberOfIndividuals, std::numeric_limits<Spec::Fitness>::max());
		maximumOfIndividuals.resize(numberOfIndividuals, std::numeric_limits<Spec::Fitness>::lowest());
		meanOfIndividuals.resize(numberOfIndividuals, std::numeric_limits<Spec::Fitness>::max());
		totalOfIndividuals.resize(numberOfIndividuals, 0.0);
		countOfIndividuals.resize(numberOfIndividuals, 0);
		for (std::size_t i(0); i != individuals; ++i) {
			minimumOfIndividuals.at(i) = std::min(minimumOfIndividuals.at(i), fitnesses.at(i));
			maximumOfIndividuals.at(i) = std::max(maximumOfIndividuals.at(i), fitnesses.at(i));
			totalOfIndividuals.at(i) += fitnesses.at(i);
			++countOfIndividuals.at(i);
		}
	}
	for (std::size_t i(0); i != numberOfIndividuals; ++i) {
		meanOfIndividuals.at(i) = totalOfIndividuals.at(i) / double(countOfIndividuals.at(i));
	}
	GenealogyFitnessPlotDatum gFPD{
		.minimumOfGenerations = minimumOfGenerations,
		.maximumOfGenerations = maximumOfGenerations,
		.meanOfGenerations = meanOfGenerations,
		.numberOfGenerations = numberOfGenerations,
		.minimumOfIndividuals = minimumOfIndividuals,
		.maximumOfIndividuals = maximumOfIndividuals,
		.meanOfIndividuals = meanOfIndividuals,
		.numberOfIndividuals = numberOfIndividuals
	};
	genealogyFitnessPlotData = gFPD;
}
