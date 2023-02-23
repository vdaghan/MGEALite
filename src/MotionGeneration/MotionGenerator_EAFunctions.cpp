// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/MotionGenerator.h"

#include "MotionGeneration/Initialisers/Initialisers.h"
#include "MotionGeneration/Metrics/Metrics.h"
#include "MotionGeneration/ParentSelectors/ParentSelectors.h"
#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"
#include "MotionGeneration/Variations/Variations.h"
#include "Wavelet/HaarWavelet.h"

#include "Logging/SpdlogCommon.h"

#include <DTimer/DTimer.h>

#include <cmath>
#include <valarray>

void MotionGenerator::setupStandardFunctions(MotionParameters motionParameters) {
	ea.functions.genesis.defineParametrised("MGEAGenesisBoundary", std::bind_front(MGEA::genesisBoundary, motionParameters), {});
	ea.functions.genesis.defineParametrised("MGEAGenesisBoundaryWavelet", std::bind_front(MGEA::genesisBoundaryWavelet, motionParameters), {});
	ea.functions.genesis.defineParametrisable("MGEAGenesisRandom", std::bind_front(MGEA::genesisRandom, motionParameters));
	ea.functions.genesis.defineParametrised("MGEAGenesisZero", std::bind_front(MGEA::genesisZero, motionParameters), {});

	ea.functions.parentSelection.defineParametrisable("MGEAParentSelectorMetricProportional", MGEA::metricProportional);

	ea.functions.survivorSelection.defineParametrised("MGEASurvivorSelectorCullEquals", MGEA::cullEquals, {});
	ea.functions.survivorSelection.defineParametrisable("MGEASurvivorSelectorCullPartiallyDominated", MGEA::cullPartiallyDominated);
	ea.functions.survivorSelection.defineParametrisable("MGEASurvivorSelectorParetoFront", MGEA::paretoFront);
	//ea.functions.survivorSelection.defineParametrisable("MGEASurvivorSelectorCullEquals", MGEA::survivorSelectionOverMetric);

	auto parametrisedVariationFromIPtrsLambda = [&](std::string functionName, auto function) {
		ea.functions.variationFromIndividualPtrs.defineParametrised(functionName, std::bind_front(function, motionParameters), {});
	};
	auto parametrisableVariationFromIPtrsLambda = [&](std::string functionName, auto function) {
		ea.functions.variationFromIndividualPtrs.defineParametrisable(functionName, std::bind_front(function, motionParameters));
	};

	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsCrossoverAll", MGEA::crossoverAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsCrossoverSingle", MGEA::crossoverSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsCutAndCrossfillAll", MGEA::cutAndCrossfillAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsCutAndCrossfillSingle", MGEA::cutAndCrossfillSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsDeletionAll", MGEA::deletionAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsDeletionSingle", MGEA::deletionSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsInsertionAll", MGEA::insertionAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsInsertionSingle", MGEA::insertionSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsKPointCrossoverAll", MGEA::kPointCrossoverAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsKPointCrossoverSingle", MGEA::kPointCrossoverSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsSNV", MGEA::snv);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsUniformCrossoverAll", MGEA::uniformCrossoverAll);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsUniformCrossoverSingle", MGEA::uniformCrossoverSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsWaveletSNV", MGEA::waveletSNV);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsHalfSineAsynchronous", MGEA::halfSineAsynchronous);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsHalfSineSingle", MGEA::halfSineSingle);
	parametrisedVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsHalfSineSynchronous", MGEA::halfSineSynchronous);
	parametrisableVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsDeletionLInt", MGEA::deletionLInt);
	parametrisableVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsDirectionalLInt", MGEA::directionalLInt);
	parametrisableVariationFromIPtrsLambda("MGEAVariationFromIndividualPtrsSNVLInt", MGEA::snvLInt);
}

Spec::MaybePhenotype MotionGenerator::transform(Spec::Genotype genotype) {
	return datastore.simulate(genotype);
}

//Spec::MetricVariantMap MotionGenerator::evaluateIndividualFromGenotypeProxy(Spec::GenotypeProxy genPx) {
//	//auto & timer = DTimer::simple("evaluate()").newSample().begin();
//	auto simLogPtr = database.getSimulationLog(genPx);
//	if (simLogPtr->fitnessExists()) {
//		//timer.end();
//		return simLogPtr->data()->metrics;
//	}
//	SimulationDataPtr simDataPtr = simLogPtr->data();
//
//	bool hasHeelHeight = simDataPtr->outputs.contains("heelZ");
//	bool hasToeHeight = simDataPtr->outputs.contains("toeZ");
//	bool hasFingertipHeight = simDataPtr->outputs.contains("fingertipZ");
//	bool hasPalmHeight = simDataPtr->outputs.contains("palmZ");
//	if (!hasToeHeight or !hasFingertipHeight or !hasHeelHeight or !hasPalmHeight) {
//		spdlog::error("There is no position named \"heelZ\" or \"toeZ\" or \"fingertipZ\" or \"palmZ\" in simulation output {}", genPx);
//		//timer.end();
//		return {};
//	}
//	double timeStep = motionParameters.simStep;
//	auto & fingertipZ = simDataPtr->outputs.at("fingertipZ");
//	auto & palmX = simDataPtr->outputs.at("palmX");
//	auto & palmZ = simDataPtr->outputs.at("palmZ");
//	auto & heelZ = simDataPtr->outputs.at("heelZ");
//	auto & toeZ = simDataPtr->outputs.at("toeZ");
//	auto & comX = simDataPtr->outputs.at("centerOfMassX");
//	auto & comZ = simDataPtr->outputs.at("centerOfMassZ");
//	auto & shoulderAngle = simDataPtr->angles.at("shoulder");
//	bool sameSize = fingertipZ.size() == toeZ.size()
//		and fingertipZ.size() == heelZ.size()
//		and fingertipZ.size() == palmZ.size();
//	if (!sameSize) {
//		spdlog::error("\"heelZ\", \"toeZ\", \"palmZ\" and \"fingertipHeight\" are not of same size");
//		//timer.end();
//		return {};
//	}
//
//	Spec::MetricVariantMap metrics;
//
//	auto absLambda = [](double prev, double next) {
//		return prev + std::abs(next);
//	};
//	auto onlyPositiveLambda = [](double prev, double next) {
//		return prev + std::max(next, 0.0);
//	};
//	//double fingertipZSum = std::accumulate(fingertipZ.begin(), fingertipZ.end(), 0.0, onlyPositiveLambda);
//	//double palmZSum = std::accumulate(palmZ.begin(), palmZ.end(), 0.0, onlyPositiveLambda);
//	double fingertipZSum = std::accumulate(fingertipZ.begin(), fingertipZ.end(), 0.0);
//	double maxFingertipZ = *std::max_element(fingertipZ.begin(), fingertipZ.end());
//	double palmZSum = std::accumulate(palmZ.begin(), palmZ.end(), 0.0);
//	double maxPalmZ = *std::max_element(palmZ.begin(), palmZ.end());
//	double heelZSum = std::accumulate(heelZ.begin(), heelZ.end(), 0.0, absLambda);
//	double heelZMax = *std::max_element(heelZ.begin(), heelZ.end());
//	double toeZSum = std::accumulate(toeZ.begin(), toeZ.end(), 0.0, absLambda);
//	double toeZMax = *std::max_element(toeZ.begin(), toeZ.end());
//	double comXSum = std::accumulate(comX.begin(), comX.end(), 0.0, absLambda);
//	double comZSum = std::accumulate(comZ.begin(), comZ.end(), 0.0, absLambda);
//	double shoulderAngleDiffSum{};
//	for (auto it(shoulderAngle.begin()); it != shoulderAngle.end() and std::next(it) != shoulderAngle.end(); ++it) {
//		auto const & currentElement(*it);
//		auto const & nextElement(*std::next(it));
//		shoulderAngleDiffSum += std::min(currentElement - nextElement, 0.0);
//	}
//	double balance = std::inner_product(comX.begin(), comX.end(), palmX.begin(), 0.0, std::plus<>(), [](auto const& cX, auto const& pX) {
//		return std::abs(cX - pX);
//	});
//	//double balance(0.0);
//	//for (std::size_t i(0); i != palmX.size(); ++i) {
//	//	balance += std::abs(comX[i] - palmX[i]);
//	//}
//	balance = balance * timeStep / motionParameters.simStop();
//	double fitness;
//	if (fingertipZSum <= 0.0 and palmZSum <= 0.0) {
//		fitness = comZSum * timeStep / motionParameters.simStop();
//	} else {
//		fitness = 0.0;
//		if (fingertipZSum > 0.0) {
//			fitness -= maxFingertipZ;
//		}
//		if (palmZSum > 0.0) {
//			fitness -= maxPalmZ;
//		}
//	}
//	metrics["fingertipZSum"] = fingertipZSum;
//	metrics["palmZSum"] = palmZSum;
//	metrics["heelZSum"] = heelZSum;
//	metrics["heelZMax"] = heelZMax;
//	metrics["toeZSum"] = toeZSum;
//	metrics["toeZMax"] = toeZMax;
//	metrics["comXSum"] = comXSum;
//	metrics["comZSum"] = comZSum;
//	metrics["shoulderAngleDiffSum"] = shoulderAngleDiffSum;
//	metrics["fitness"] = fitness;
//	metrics["balance"] = balance;
//	metrics["angularVelocitySign"] = MGEA::computeAngularVelocitySign(simLogPtr->data()->angles);
//
//	simLogPtr->data()->metrics = metrics;
//	database.saveSimulationMetrics(simLogPtr->info(), metrics);
//	//spdlog::info("Individual{} evaluated to fitness value {}", genPx, fitness);
//	//timer.end();
//	return metrics;
//}

void MotionGenerator::applyMotionParameters(SimulationDataPtr sptr) {
	sptr->time = motionParameters.time();
	sptr->alignment = motionParameters.alignment;
	sptr->timeout = motionParameters.timeout;
	sptr->params.emplace("simStart", motionParameters.simStart);
	sptr->params.emplace("simStop", motionParameters.simStop());
	sptr->params.emplace("simStep", motionParameters.simStep);
	sptr->params.emplace("simSamples", static_cast<double>(motionParameters.simSamples));
	sptr->masses = motionParameters.masses;
	sptr->contacts = motionParameters.contactParameters;
}

void MotionGenerator::onEpochStart(std::size_t generation) {
	currentGeneration = generation;
	spdlog::info("Epoch {} started.", generation);
}

void MotionGenerator::onEpochEnd(std::size_t generation) {
	spdlog::info("Epoch {} ended.", generation);
	auto & lastGeneration = ea.genealogy.back();

	auto & timer = DTimer::simple("stats").newSample().begin();
	auto const & bestIndividualPtr = lastGeneration.front();
	//database.saveVisualisationTarget(bestIndividualPtr->id);

	auto & bestIndividualMetric(ea.bestIndividual->metricMap);
	double bestFitness(bestIndividualMetric.at("fitness").as<double>());
	spdlog::info("Best fitness: {}", bestFitness);

	//std::list<double> simulationTimes;
	//for (auto & iptr : lastGeneration) {
	//	if (iptr->genotypeProxy.generation != generation) {
	//		continue;
	//	}
	//	auto simulationLogPtr = database.getSimulationLog(iptr->genotypeProxy);
	//	auto const & metadata = simulationLogPtr->data()->metadata;
	//	if (metadata.contains("totalTime")) {
	//		auto const & totalTime = metadata.at("totalTime");
	//		simulationTimes.push_back(totalTime);
	//	}
	//}
	//if (!simulationTimes.empty()) {
	//	auto minmax = std::minmax_element(simulationTimes.begin(), simulationTimes.end());
	//	double total = std::accumulate(simulationTimes.begin(), simulationTimes.end(), 0.0);
	//	double mean = total / static_cast<double>(simulationTimes.size());
	//	spdlog::info("(min, mean, max) simulation times were: ({:.3f}s, {:.3f}s, {:.3f}s)", *minmax.first, mean, *minmax.second);
	//	//spdlog::info("Total simulation time was: {:.3f}s", total);
	//	spdlog::info("Total simulation time was: {}", DTimer::printTime(static_cast<std::size_t>(total * 1000.0)));
	//}
	timer.end();
	spdlog::info("\n{}", DTimer::print());
}
