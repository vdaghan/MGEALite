// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/MotionGenerator.h"

#include "MotionGeneration/Initialisers/Initialisers.h"
#include "MotionGeneration/Metrics/Metrics.h"
#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"
#include "Logging/SpdlogCommon.h"
#include "Wavelet/HaarWavelet.h"

#include "DEvA/Project/Filestore.h"
#include <DTimer/DTimer.h>

#include <algorithm>
#include <functional>

MotionGenerator::MotionGenerator(std::string dataFolder, std::string setupFile) 
: datastore(dataFolder)
{
	importMotionParameters(setupFile);
	setupStandardFunctions();
	importSetup(setupFile);
	DEvA::EvolutionaryAlgorithm<Spec>::datastore = std::make_shared<DEvA::Filestore<Spec>>();

	functions.genesis.wrapper = [&](auto genesisFunction) {
		typename Spec::Genotypes genotypes(genesisFunction());
		for (auto & genotype : genotypes) {
			applyMotionParameters(genotype);
		}
		return genotypes;
	};
	auto variationWrapper = [&](auto variationFunctor, auto parents) {
		auto children = variationFunctor(parents);
		for (auto& child : children) {
			applyMotionParameters(child);
		}
		return children;
	};
	functions.variationFromGenotypes.wrapper = variationWrapper;
	functions.variationFromIndividualPtrs.wrapper = variationWrapper;

	functions.transform.definePlain("Simulate", std::bind_front(&MotionGenerator::transform, this));
	functions.transform.compile("SimulateCompiled", { "Simulate" });

	onEpochStartCallback = std::bind_front(&MotionGenerator::onEpochStart, this);
	onEpochEndCallback = std::bind_front(&MotionGenerator::onEpochEnd, this);
	logger.callback = DEvALoggerCallback;
};

DEvA::StepResult MotionGenerator::search(std::size_t n) {
	return DEvA::EvolutionaryAlgorithm<Spec>::search(n);
}
