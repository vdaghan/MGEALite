#include "MotionGeneration/MotionGenerator.h"

#include "MotionGeneration/Variations/Variations.h"
#include "Wavelet/HaarWavelet.h"

#include "Logging/SpdlogCommon.h"

#include <DTimer/DTimer.h>

std::list<Spec::SVariationFunctor> MotionGenerator::createVariationFunctors() {
	std::list<Spec::SVariationFunctor> variationFunctors;
	
	Spec::SVariationFunctor variationFunctorCrossoverAll;
	variationFunctorCrossoverAll.name = "CrossoverAll";
	variationFunctorCrossoverAll.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	variationFunctorCrossoverAll.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::crossoverAll);
	variationFunctorCrossoverAll.probability = 1.0;
	variationFunctorCrossoverAll.removeParentsFromMatingPool = false;
	variationFunctors.push_back(variationFunctorCrossoverAll);
	
	Spec::SVariationFunctor variationFunctorCrossoverSingle;
	variationFunctorCrossoverSingle.name = "CrossoverSingle";
	variationFunctorCrossoverSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	variationFunctorCrossoverSingle.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::crossoverSingle);
	variationFunctorCrossoverSingle.probability = 1.0;
	variationFunctorCrossoverSingle.removeParentsFromMatingPool = false;
	variationFunctors.push_back(variationFunctorCrossoverSingle);
	
	Spec::SVariationFunctor variationFunctorCutAndCrossfillAll;
	variationFunctorCutAndCrossfillAll.name = "CutAndCrossfillAll";
	variationFunctorCutAndCrossfillAll.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	variationFunctorCutAndCrossfillAll.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::cutAndCrossfillAll);
	variationFunctorCutAndCrossfillAll.probability = 1.0;
	variationFunctorCutAndCrossfillAll.removeParentsFromMatingPool = false;
	variationFunctors.push_back(variationFunctorCutAndCrossfillAll);
	
	Spec::SVariationFunctor variationFunctorCutAndCrossfillSingle;
	variationFunctorCutAndCrossfillSingle.name = "CutAndCrossfillSingle";
	variationFunctorCutAndCrossfillSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	variationFunctorCutAndCrossfillSingle.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::cutAndCrossfillSingle);
	variationFunctorCutAndCrossfillSingle.probability = 1.0;
	variationFunctorCutAndCrossfillSingle.removeParentsFromMatingPool = false;
	variationFunctors.push_back(variationFunctorCutAndCrossfillSingle);
	
	Spec::SVariationFunctor variationFunctorDeletionAll;
	variationFunctorDeletionAll.name = "DeletionAll";
	variationFunctorDeletionAll.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<1, 50>;
	variationFunctorDeletionAll.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::deletionAll);
	variationFunctorDeletionAll.probability = 1.0;
	variationFunctorDeletionAll.removeParentsFromMatingPool = false;
	variationFunctors.push_back(variationFunctorDeletionAll);
	
	//Spec::SVariationFunctor variationFunctorDeletionSingle;
	//variationFunctorDeletionSingle.name = "DeletionSingle";
	//variationFunctorDeletionSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<1, 50>;
	//variationFunctorDeletionSingle.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::deletionSingle);
	//variationFunctorDeletionSingle.probability = 0.2;
	//variationFunctorDeletionSingle.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorDeletionSingle);
	
	Spec::SVariationFunctor variationFunctorInsertionAll;
	variationFunctorInsertionAll.name = "InsertionAll";
	variationFunctorInsertionAll.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<1, 50>;
	variationFunctorInsertionAll.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::insertionAll);
	variationFunctorInsertionAll.probability = 1.0;
	variationFunctorInsertionAll.removeParentsFromMatingPool = false;
	variationFunctors.push_back(variationFunctorInsertionAll);
	
	Spec::SVariationFunctor variationFunctorInsertionSingle;
	variationFunctorInsertionSingle.name = "InsertionSingle";
	variationFunctorInsertionSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<1, 50>;
	variationFunctorInsertionSingle.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::insertionSingle);
	variationFunctorInsertionSingle.probability = 1.0;
	variationFunctorInsertionSingle.removeParentsFromMatingPool = false;
	variationFunctors.push_back(variationFunctorInsertionSingle);
	
	Spec::SVariationFunctor variationFunctorkPointCrossoverAll;
	variationFunctorkPointCrossoverAll.name = "kPointCrossoverAll";
	variationFunctorkPointCrossoverAll.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	variationFunctorkPointCrossoverAll.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::kPointCrossoverAll);
	variationFunctorkPointCrossoverAll.probability = 1.0;
	variationFunctorkPointCrossoverAll.removeParentsFromMatingPool = false;
	variationFunctors.push_back(variationFunctorkPointCrossoverAll);
	
	//Spec::SVariationFunctor variationFunctorkPointCrossoverSingle;
	//variationFunctorkPointCrossoverSingle.name = "kPointCrossoverSingle";
	//variationFunctorkPointCrossoverSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	//variationFunctorkPointCrossoverSingle.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::kPointCrossoverSingle);
	//variationFunctorkPointCrossoverSingle.probability = 1.0;
	//variationFunctorkPointCrossoverSingle.removeParentsFromMatingPool = false;
	//variationFunctors.push_back(variationFunctorkPointCrossoverSingle);
	
	Spec::SVariationFunctor variationFunctorSNV;
	variationFunctorSNV.name = "SNV";
	variationFunctorSNV.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<1, 10>;
	variationFunctorSNV.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::snv);
	variationFunctorSNV.probability = 1.0;
	variationFunctorSNV.removeParentsFromMatingPool = false;
	variationFunctors.push_back(variationFunctorSNV);
	
	Spec::SVariationFunctor variationFunctorUniformCrossoverAll;
	variationFunctorUniformCrossoverAll.name = "UniformCrossoverAll";
	variationFunctorUniformCrossoverAll.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	variationFunctorUniformCrossoverAll.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::uniformCrossoverAll);
	variationFunctorUniformCrossoverAll.probability = 1.0;
	variationFunctorUniformCrossoverAll.removeParentsFromMatingPool = false;
	variationFunctors.push_back(variationFunctorUniformCrossoverAll);
	
	Spec::SVariationFunctor variationFunctorUniformCrossoverSingle;
	variationFunctorUniformCrossoverSingle.name = "UniformCrossoverSingle";
	variationFunctorUniformCrossoverSingle.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>;
	variationFunctorUniformCrossoverSingle.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::uniformCrossoverSingle);
	variationFunctorUniformCrossoverSingle.probability = 1.0;
	variationFunctorUniformCrossoverSingle.removeParentsFromMatingPool = false;
	variationFunctors.push_back(variationFunctorUniformCrossoverSingle);
	
	//Spec::SVariationFunctor variationFunctorWaveletSNV;
	//variationFunctorWaveletSNV.name = "variationFunctorWaveletSNV";
	//variationFunctorWaveletSNV.parentSelectionFunction = DEvA::StandardParentSelectors<Spec>::bestNofAll<1>;
	//variationFunctorWaveletSNV.variationFunction = std::bind_front(&MotionGenerator::computeVariation, this, &MGEA::waveletSNV);
	//variationFunctorWaveletSNV.probability = 1.0;
	//variationFunctorWaveletSNV.removeParentsFromMatingPool = true;
	//variationFunctors.push_back(variationFunctorWaveletSNV);
	
	return variationFunctors;
}

Spec::MaybePhenotypeProxy MotionGenerator::transform(Spec::GenotypeProxy genPx) {
	auto simLogPtr = database.getSimulationLog(genPx);
	if (simLogPtr->outputExists()) {
		return genPx;
	}
	auto maybeSimulationDataPtr = database.requestSimulationResult(simLogPtr->info()).get();
	if (std::unexpected(MGEA::ErrorCode::SimulationError) == maybeSimulationDataPtr) {
		return std::unexpected(DEvA::ErrorCode::InvalidTransform);
	}
	if (maybeSimulationDataPtr and maybeSimulationDataPtr.value()) {
		return genPx;
	}
	return std::unexpected(DEvA::ErrorCode::InvalidTransform);
}

Spec::Fitness MotionGenerator::evaluate(Spec::GenotypeProxy genPx) {
	//auto & timer = DTimer::simple("evaluate()").newSample().begin();
	auto simLogPtr = database.getSimulationLog(genPx);
	if (simLogPtr->fitnessExists()) {
		//timer.end();
		return simLogPtr->data()->fitness;
	}
	SimulationDataPtr simDataPtr = simLogPtr->data();

	bool hasHeelHeight = simDataPtr->outputs.contains("heelHeight");
	bool hasToeHeight = simDataPtr->outputs.contains("toeHeight");
	bool hasFingertipHeight = simDataPtr->outputs.contains("fingertipHeight");
	bool hasPalmHeight = simDataPtr->outputs.contains("palmHeight");
	if (!hasToeHeight or !hasFingertipHeight or !hasHeelHeight or !hasPalmHeight) {
		spdlog::error("There is no position named \"heelHeight\" or \"toeHeight\" or \"fingertipHeight\" or \"palmHeight\" in simulation output {}", genPx);
		//timer.end();
		return 0.0;
	}
	double timeStep = motionParameters.simStep;
	auto & fingertipHeight = simDataPtr->outputs.at("fingertipHeight");
	auto & palmHeight = simDataPtr->outputs.at("palmHeight");
	auto & heelHeight = simDataPtr->outputs.at("heelHeight");
	auto & toeHeight = simDataPtr->outputs.at("toeHeight");
	bool sameSize = fingertipHeight.size() == toeHeight.size()
		and fingertipHeight.size() == heelHeight.size()
		and fingertipHeight.size() == palmHeight.size();
	if (!sameSize) {
		spdlog::error("\"heelHeight\", \"toeHeight\", \"palmHeight\" and \"fingertipHeight\" are not of same size");
		//timer.end();
		return 0.0;
	}
	//double diff(0.0);
	double fingertipHeightSum = std::accumulate(fingertipHeight.begin(), fingertipHeight.end(), 0.0);
	double palmHeightSum = std::accumulate(palmHeight.begin(), palmHeight.end(), 0.0);
	double heelHeightSum = std::accumulate(heelHeight.begin(), heelHeight.end(), 0.0);
	double toeHeightSum = std::accumulate(toeHeight.begin(), toeHeight.end(), 0.0);
	//double fitness = diff * timeStep - std::abs(fingertipHeightSum);
	double fitness;
	if (fingertipHeightSum <= 0.0 and palmHeightSum <= 0.0) {
		fitness = toeHeightSum * timeStep;
	} else {
		fitness = 0.0;
		if (fingertipHeightSum > 0.0) {
			fitness -= fingertipHeightSum;
		}
		if (palmHeightSum > 0.0) {
			fitness -= palmHeightSum;
		}
	}
	simLogPtr->data()->fitness = fitness;
	database.setSimulationFitness(simLogPtr->info(), fitness);
	//spdlog::info("Individual{} evaluated to fitness value {}", genPx, fitness);
	//timer.end();
	return fitness;
}

Spec::Distance MotionGenerator::calculateDistance(DEvA::IndividualIdentifier id1, DEvA::IndividualIdentifier id2) {
	auto simLogPtr1(database.getSimulationLog(id1));
	auto simLogPtr2(database.getSimulationLog(id2));
	auto simDataPtr1(simLogPtr1->data());
	auto simDataPtr2(simLogPtr2->data());

	auto numSamples(motionParameters.simSamples);
	auto jointNames(motionParameters.jointNames);
	std::size_t distance(0);
	for (auto& jointName : jointNames) {
		auto& torque1(simDataPtr1->torque.at(jointName));
		auto& torque2(simDataPtr2->torque.at(jointName));
		for (std::size_t i(0); i != numSamples; ++i) {
			auto t1(torque1.at(i));
			auto t2(torque2.at(i));
			if (0.0 == t1 or 0.0 == t2) {
				continue;
			}
			if (t1 * t2 < 0.0) {
				++distance;
			}
		}
	}
	return distance;
}

bool MotionGenerator::convergenceCheck(Spec::Fitness f) {
	return f > 1.5 * motionParameters.simStop();
}

void MotionGenerator::applyMotionParameters(SimulationDataPtr sptr) {
	sptr->time = motionParameters.time();
	sptr->params.emplace("simStart", motionParameters.simStart);
	sptr->params.emplace("simStop", motionParameters.simStop());
	sptr->params.emplace("simStep", motionParameters.simStep);
	sptr->params.emplace("simSamples", static_cast<double>(motionParameters.simSamples));
	sptr->alignment = motionParameters.alignment;
	sptr->timeout = motionParameters.timeout;
	sptr->contacts = motionParameters.contactParameters;
}

Spec::GenotypeProxies MotionGenerator::computeVariation(std::function<SimulationDataPtrs(MGEA::VariationParams, SimulationDataPtrs)> vFunc, Spec::GenotypeProxies parentProxies) {
	std::size_t const simLength = motionParameters.simSamples;
	std::size_t const numJoints = motionParameters.jointNames.size();
	if (0 == simLength || 0 == numJoints) {
		return {};
	}

	auto& timer = DTimer::simple("computeVariation()").newSample().begin();
	std::list<SimulationDataPtr> parentData;
	for (auto& parentProxy : parentProxies) {
		SimulationLogPtr parentLogPtr = database.getSimulationLog(parentProxy);
		SimulationDataPtr parentDataPtr = parentLogPtr->data();
		parentData.push_back(parentDataPtr);
	}

	MGEA::VariationParams variationParameters{
		.motionParameters = motionParameters,
		.pauseFlag = pauseFlag,
		.stopFlag = stopFlag
	};

	auto children = vFunc(variationParameters, parentData);

	Spec::GenotypeProxies childProxies;
	for (auto& child : children) {
		if (checkStopFlagAndMaybeWait()) {
			return {};
		}
		applyMotionParameters(child);
		auto childIndividualIdentifier = ea.reserveNewIndividualIdentifier();
		//auto childIndividualPtr = ea.createNewIndividual(childIndividualIdentifier);
		auto childLogPtr = database.registerSimulation(childIndividualIdentifier);
		// TODO Check if we could successfully create a new input?
		updateSimulationDataPtr({ .source = child, .target = childLogPtr->data() });
		MGEA::ErrorCode startError = database.startSimulation(childIndividualIdentifier);
		// TODO Check if start was successful?
		childProxies.push_back(childIndividualIdentifier);
	}
	timer.end();
	return childProxies;
}

Spec::GenotypeProxies MotionGenerator::computeGenesis(std::function<SimulationDataPtrs(MGEA::InitialiserParams)> gFunc) {
	MGEA::InitialiserParams initialiserParams{
		.motionParameters = motionParameters,
		.pauseFlag = pauseFlag,
		.stopFlag = stopFlag,
		.database = database
	};

	auto simDataPtrs(gFunc(initialiserParams));

	Spec::GenotypeProxies genotypeProxies;
	for (auto& simDataPtr : simDataPtrs) {
		applyMotionParameters(simDataPtr);
		auto individualIdentifier = ea.reserveNewIndividualIdentifier();
		auto logPtr = database.registerSimulation(individualIdentifier);
		*logPtr->data() = *simDataPtr;
		MGEA::ErrorCode startError = database.startSimulation(individualIdentifier);
		genotypeProxies.push_back(individualIdentifier);
	}

	return genotypeProxies;
}

void MotionGenerator::onEpochStart(std::size_t generation) {
	currentGeneration = generation;
	spdlog::info("Epoch {} started.", generation);
	updateMotionGenerationStateWithEAProgress();
}

void MotionGenerator::onEpochEnd(std::size_t generation) {
	spdlog::info("Epoch {} ended.", generation);
	auto & lastGeneration = ea.genealogy.back();

	updateMotionGenerationStateWithEAProgress();
	updateMotionGenerationStateWithFitnessStatus();

	auto & timer = DTimer::simple("stats").newSample().begin();
	auto const & bestIndividualPtr = lastGeneration.front();
	auto worstIndividualPtr = lastGeneration.front();
	std::list<Spec::Fitness> fitnessValues;
	Spec::Fitness totalFitness = 0.0;
	for (auto const & iptr : lastGeneration) {
		if (iptr->maybePhenotypeProxy) {
			auto & fitness = iptr->fitness;
			totalFitness += fitness;
			fitnessValues.push_back(fitness);
			worstIndividualPtr = iptr;
		}
	}
	Spec::Fitness meanFitness = totalFitness / static_cast<Spec::Fitness>(fitnessValues.size());
	spdlog::info("Best individual {} has fitness {}.", bestIndividualPtr->genotypeProxy, bestIndividualPtr->fitness);
	spdlog::info("Worst individual {} has fitness {}.", worstIndividualPtr->genotypeProxy, worstIndividualPtr->fitness);
	spdlog::info("Mean fitness value: {}.", meanFitness);
	database.saveVisualisationTarget(bestIndividualPtr->genotypeProxy);

	std::list<double> simulationTimes;
	for (auto & iptr : lastGeneration) {
		if (iptr->genotypeProxy.generation != generation) {
			continue;
		}
		auto simulationLogPtr = database.getSimulationLog(iptr->genotypeProxy);
		auto const & metadata = simulationLogPtr->data()->metadata;
		if (metadata.contains("totalTime")) {
			auto const & totalTime = metadata.at("totalTime");
			simulationTimes.push_back(totalTime);
		}
	}
	if (!simulationTimes.empty()) {
		auto minmax = std::minmax_element(simulationTimes.begin(), simulationTimes.end());
		double total = std::accumulate(simulationTimes.begin(), simulationTimes.end(), 0.0);
		double mean = total / static_cast<double>(simulationTimes.size());
		spdlog::info("(min, mean, max) simulation times were: ({:.3f}s, {:.3f}s, {:.3f}s)", *minmax.first, mean, *minmax.second);
		//spdlog::info("Total simulation time was: {:.3f}s", total);
		spdlog::info("Total simulation time was: {}", DTimer::printTime(static_cast<std::size_t>(total * 1000.0)));
	}
	timer.end();
	spdlog::info("\n{}", DTimer::print());
}
