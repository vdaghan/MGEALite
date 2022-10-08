#pragma once

#include "MotionGeneration/MotionGenerator.h"
#include "EvolutionaryAlgorithm.h"

#include <deque>
#include <list>
#include <memory>
#include <mutex>
#include <vector>
#include <utility>

class GUIState {
	public:
		Spec::Genotype bestGenotype;
		std::list<std::pair<Spec::Genotype, Spec::Genotype>> thisGenerationGenotypeInterval;

		std::vector<Spec::Fitness> minFitness;
		std::vector<Spec::Fitness> maxFitness;
		std::vector<Spec::Fitness> lastFitness;
		std::vector<Spec::Fitness> minFitnesses;
		std::vector<Spec::Fitness> maxFitnesses;
		std::vector<Spec::Fitness> meanFitnesses;
		
		std::mutex & getMutexRef();

		void updateGenealogy(Spec::Genealogy const &);
		void updateGenotypes() {};
		void updateVariationInfos() {};
	private:
		std::mutex updateLock;
		std::vector<std::vector<Spec::Fitness>> allGenerationFitness;
		void calculateFitnesses();
};

using GUIStatePtr = std::shared_ptr<GUIState>;
