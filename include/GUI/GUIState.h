#pragma once

#include "MotionGeneration/MotionGenerator.h"
#include "MotionGeneration/MotionGenerationState.h"
#include "MotionGeneration/Specification.h"
#include "EvolutionaryAlgorithm.h"

#include <deque>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>
#include <utility>

struct GenerationProgressPlotDatum {
	std::size_t failed;
	std::size_t evaluated;
	std::size_t total;
};

struct GenerationFitnessPlotDatum {
	std::vector<Spec::Fitness> fitnesses; // Indexed by individuals
	Spec::Fitness minimum;
	Spec::Fitness maximum;
	Spec::Fitness mean;
	std::size_t count; // Number of individuals
};

struct GenealogyFitnessPlotDatum {
	std::vector<Spec::Fitness> minimumOfGenerations; // Indexed by generation
	std::vector<Spec::Fitness> maximumOfGenerations; // Indexed by generation
	std::vector<Spec::Fitness> meanOfGenerations; // Indexed by generation
	std::size_t numberOfGenerations; // Number of generations
	std::vector<Spec::Fitness> minimumOfIndividuals; // Indexed by individuals
	std::vector<Spec::Fitness> maximumOfIndividuals; // Indexed by individuals
	std::vector<Spec::Fitness> meanOfIndividuals; // Indexed by individuals
	std::size_t numberOfIndividuals; // Number of individuals
};

class GUIState {
	public:
		void updateMotionGenerationState(std::size_t, MotionGenerationState const &);

		std::size_t generations();
		std::optional<GenerationProgressPlotDatum> generationProgressPlotDatum(std::size_t) const;
		std::optional<GenerationFitnessPlotDatum> generationFitnessPlotDatum(std::size_t) const;
		std::optional<GenealogyFitnessPlotDatum> genealogyFitnessPlotDatum(std::size_t) const;
	private:
		mutable std::mutex updateMutex;
		std::deque<MotionGenerationState> motionGenerationStates;
		std::deque<GenerationProgressPlotDatum> generationProgressPlotData;
		void updateGenerationProgressPlotDatum(std::size_t);
		std::deque<GenerationFitnessPlotDatum> generationFitnessPlotData;
		void updateGenerationFitnessPlotDatum(std::size_t);
		GenealogyFitnessPlotDatum genealogyFitnessPlotData;
		void updateGenealogyFitnessPlotDatum();
};

using GUIStatePtr = std::shared_ptr<GUIState>;
