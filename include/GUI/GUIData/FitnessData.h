#pragma once

#include "MotionGeneration/Specification.h"
#include "EvolutionaryAlgorithm.h"

#include <mutex>
#include <vector>

namespace MGEA {
	struct FitnessData {
		FitnessData() : nextGeneration(0), numberOfGenerations(0) {};

		// Fitness vs Individuals
		std::vector<double> lastGenerationFitnesses;
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
			//auto const & individualMetrics(eaStatistics.individualMetrics);

			//lastGenerationFitnesses = {};
			//std::copy(individualMetrics.begin(), individualMetrics.end(), std::back_inserter(lastGenerationFitnesses));
			//if (individualMetrics.empty()) {
			//	lastGenerationMinimumFitness = 0.0;
			//	lastGenerationMaximumFitness = 0.0;
			//} else {
			//	//lastGenerationMinimumFitness = *std::min_element(individualMetrics.begin(), individualMetrics.end());
			//	//lastGenerationMaximumFitness = *std::max_element(individualMetrics.begin(), individualMetrics.end());
			//}
			//lastGenerationDiff = lastGenerationMaximumFitness - lastGenerationMinimumFitness;
			//lastGenerationTotalFitness = std::accumulate(individualMetrics.begin(), individualMetrics.end(), 0.0);
			//lastGenerationNumberOfIndividuals = individualMetrics.size();
			//lastGenerationMeanFitness = lastGenerationTotalFitness / static_cast<double>(lastGenerationNumberOfIndividuals);

			//minimumOfGenerations.push_back(lastGenerationMinimumFitness);
			//maximumOfGenerations.push_back(lastGenerationMaximumFitness);
			//meanOfGenerations.push_back(lastGenerationMeanFitness);
			//++numberOfGenerations;

			//++nextGeneration;
		};
	};
};
