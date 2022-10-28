#pragma once

#include "MotionGeneration/Specification.h"
#include "EvolutionaryAlgorithm.h"

#include <mutex>
#include <vector>

namespace MGEA {
	struct FitnessData {
		FitnessData() : nextGeneration(0), numberOfGenerations(0) {};

		// Fitness vs Individuals
		std::vector<Spec::Fitness> lastGenerationFitnesses;
		double lastGenerationMinimumFitness;
		double lastGenerationMaximumFitness;
		double lastGenerationDiff;
		double lastGenerationTotalFitness;
		std::size_t lastGenerationNumberOfIndividuals;
		double lastGenerationMeanFitness;
		std::size_t nextGeneration;

		// Fitness vs Generations
		std::vector<double> minimumOfGenerations;
		std::vector<double> maximumOfGenerations;
		std::vector<double> meanOfGenerations;
		std::size_t numberOfGenerations;


		void update(DEvA::EAStatistics<Spec> const & eaStatistics) {
			auto const & fitnesses(eaStatistics.fitnesses);

			lastGenerationFitnesses = {};
			std::copy(fitnesses.begin(), fitnesses.end(), std::back_inserter(lastGenerationFitnesses));
			if (fitnesses.empty()) {
				lastGenerationMinimumFitness = 0.0;
				lastGenerationMaximumFitness = 0.0;
			} else {
				lastGenerationMinimumFitness = *std::min_element(fitnesses.begin(), fitnesses.end());
				lastGenerationMaximumFitness = *std::max_element(fitnesses.begin(), fitnesses.end());
			}
			lastGenerationDiff = lastGenerationMaximumFitness - lastGenerationMinimumFitness;
			lastGenerationTotalFitness = std::accumulate(fitnesses.begin(), fitnesses.end(), 0.0);
			lastGenerationNumberOfIndividuals = fitnesses.size();
			lastGenerationMeanFitness = lastGenerationTotalFitness / static_cast<double>(lastGenerationNumberOfIndividuals);

			minimumOfGenerations.push_back(lastGenerationMinimumFitness);
			maximumOfGenerations.push_back(lastGenerationMaximumFitness);
			meanOfGenerations.push_back(lastGenerationMeanFitness);
			++numberOfGenerations;

			++nextGeneration;
		};
	};
};
