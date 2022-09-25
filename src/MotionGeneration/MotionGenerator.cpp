#include "MotionGeneration/MotionGenerator.h"
#include "MotionGeneration/Variations/CrossoverAll.h"
#include "MotionGeneration/Variations/CrossoverSingle.h"
#include "MotionGeneration/Variations/CutAndCrossfillAll.h"
#include "MotionGeneration/Variations/CutAndCrossfillSingle.h"
#include "MotionGeneration/Variations/SNV.h"
#include "MotionGeneration/Variations/WaveletSNV.h"
#include "Logging/SpdlogCommon.h"

#include <DTimer/DTimer.h>

#include <algorithm>
#include <functional>

MotionGenerator::MotionGenerator(std::string folder, MotionParameters mP) : motionParameters(mP), database(folder, motionParameters) {
	ea.setGenesisFunction(std::bind_front(&MotionGenerator::genesisRandom, this, 256));
	ea.setTransformFunction(std::bind_front(&MotionGenerator::transform, this));
	ea.setEvaluationFunction(std::bind_front(&MotionGenerator::evaluate, this));
	ea.setFitnessComparisonFunction([](Spec::Fitness lhs, Spec::Fitness rhs){ return lhs > rhs; });
	Spec::SVariationFunctor variationFunctorCrossoverAll;
	variationFunctorCrossoverAll.setParentSelectionFunction(DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>);
	variationFunctorCrossoverAll.setVariationFunction(std::bind_front(&MotionGenerator::computeVariation, this, &crossoverAll));
	variationFunctorCrossoverAll.setProbability(0.25);
	variationFunctorCrossoverAll.setRemoveParentFromMatingPool(false);
	ea.addVariationFunctor(variationFunctorCrossoverAll);
	Spec::SVariationFunctor variationFunctorCrossoverSingle;
	variationFunctorCrossoverSingle.setParentSelectionFunction(DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>);
	variationFunctorCrossoverSingle.setVariationFunction(std::bind_front(&MotionGenerator::computeVariation, this, &crossoverSingle));
	variationFunctorCrossoverSingle.setProbability(0.25);
	variationFunctorCrossoverSingle.setRemoveParentFromMatingPool(false);
	ea.addVariationFunctor(variationFunctorCrossoverSingle);
	Spec::SVariationFunctor variationFunctorCutAndCrossfillAll;
	variationFunctorCutAndCrossfillAll.setParentSelectionFunction(DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>);
	variationFunctorCutAndCrossfillAll.setVariationFunction(std::bind_front(&MotionGenerator::computeVariation, this, &cutAndCrossfillAll));
	variationFunctorCutAndCrossfillAll.setProbability(0.1);
	variationFunctorCutAndCrossfillAll.setRemoveParentFromMatingPool(false);
	ea.addVariationFunctor(variationFunctorCutAndCrossfillAll);
	Spec::SVariationFunctor variationFunctorCutAndCrossfillSingle;
	variationFunctorCutAndCrossfillSingle.setParentSelectionFunction(DEvA::StandardParentSelectors<Spec>::bestNofM<2, 10>);
	variationFunctorCutAndCrossfillSingle.setVariationFunction(std::bind_front(&MotionGenerator::computeVariation, this, &cutAndCrossfillSingle));
	variationFunctorCutAndCrossfillSingle.setProbability(0.1);
	variationFunctorCutAndCrossfillSingle.setRemoveParentFromMatingPool(false);
	ea.addVariationFunctor(variationFunctorCutAndCrossfillSingle);
	Spec::SVariationFunctor variationFunctorSNV;
	variationFunctorSNV.setParentSelectionFunction(DEvA::StandardParentSelectors<Spec>::bestNofAll<1>);
	variationFunctorSNV.setVariationFunction(std::bind_front(&MotionGenerator::computeVariation, this, &snv));
	variationFunctorSNV.setProbability(1.0);
	variationFunctorSNV.setRemoveParentFromMatingPool(true);
	ea.addVariationFunctor(variationFunctorSNV);
	Spec::SVariationFunctor variationFunctorWaveletSNV;
	variationFunctorWaveletSNV.setParentSelectionFunction(DEvA::StandardParentSelectors<Spec>::bestNofAll<1>);
	variationFunctorWaveletSNV.setVariationFunction(std::bind_front(&MotionGenerator::computeVariation, this, &waveletSNV));
	variationFunctorWaveletSNV.setProbability(1.0);
	variationFunctorWaveletSNV.setRemoveParentFromMatingPool(true);
	ea.addVariationFunctor(variationFunctorWaveletSNV);
	ea.setSurvivorSelectionFunction(DEvA::StandardSurvivorSelectors<Spec>::clamp<256>);
	ea.setConvergenceCheckFunction(std::bind_front(&MotionGenerator::convergenceCheck, this));

	ea.setOnEpochStartCallback(std::bind_front(&MotionGenerator::onEpochStart, this));
	ea.setOnEpochEndCallback(std::bind_front(&MotionGenerator::onEpochEnd, this));
	ea.setLambda(250);
	exportGenerationData();
};

void MotionGenerator::exportGenerationData() {
	spdlog::info("Exporting previous data to EA...");
	SimulationHistory const & simulationHistory = database.getSimulationHistory();
	if (simulationHistory.empty()) {
		spdlog::info("No previous data found.");
		return;
	}
	auto const & lastElement = std::max_element(simulationHistory.begin(), simulationHistory.end(), [](auto & lhs, auto & rhs){ return lhs.first < rhs.first; });
	std::size_t lastGeneration = lastElement->first.generation;
	spdlog::info("Last run had {} generations.", lastGeneration);

	for (std::size_t gen(0); gen <= lastGeneration; ++gen) {
		Spec::Generation generation;
		for (auto const & historyPair : simulationHistory) {
			auto const & simInfo = historyPair.first;
			if (simInfo.generation != gen) {
				continue;
			}
			generation.emplace_back(new Spec::SIndividual(simInfo));
		}
		ea.addGeneration(generation);
	}
}

DEvA::StepResult MotionGenerator::search(std::size_t n) {
	return ea.search(n);
}

Spec::Generation MotionGenerator::genesisBoundary() {
	Spec::Generation retVal;

	std::vector<double> time(motionParameters.simSamples);
	auto timeGenerator = [this, t = motionParameters.simStart]() mutable {
		return (t += motionParameters.simStep);
	};
	std::generate(time.begin(), time.end(), timeGenerator);

	auto generateBoundaryVector = [&](std::size_t timeIndex, double value) -> std::vector<double> {
		std::vector<double> retVal(motionParameters.simSamples, 0.0);
		retVal[timeIndex] = value;
		return retVal;
	};

	std::vector<double> const zeroVector(motionParameters.simSamples, 0.0);
	std::size_t numJoints(motionParameters.jointNames.size());
	std::size_t id(0);
	for (std::size_t jointIndex(0); jointIndex != numJoints; ++jointIndex) {
		std::string const & jointName = motionParameters.jointNames[jointIndex];
		for (std::size_t timeIndex(0); timeIndex != motionParameters.simSamples; ++timeIndex) {
			auto & jointLimitsPair = motionParameters.jointLimits.at(jointName);
			std::array<double, 2> jointLimitsArray{jointLimitsPair.first, jointLimitsPair.second};
			for (auto & jointLimit : jointLimitsArray) {
				SimulationInfo simInfo{.generation = 0, .identifier = id++};
				auto simDataPtr = database.createSimulation(simInfo);
				simDataPtr->time = time;
				simDataPtr->params.emplace("simStart", motionParameters.simStart);
				simDataPtr->params.emplace("simStop", motionParameters.simStop());
				simDataPtr->params.emplace("simStep", motionParameters.simStep);
				simDataPtr->params.emplace("simSamples", static_cast<double>(motionParameters.simSamples));
				for (auto & jN : motionParameters.jointNames) {
					if (jointName != jN) {
						simDataPtr->torque.emplace(std::make_pair(jN, zeroVector));
					} else {
						auto boundaryVector = generateBoundaryVector(timeIndex, jointLimit);
						simDataPtr->torque.emplace(std::make_pair(jN, boundaryVector));
					}
				}
				SimulationLogPtr simulationLogPtr = database.getSimulationLog(simInfo);
				MGEA::ErrorCode startError = database.startSimulation(simulationLogPtr->info());
				// TODO: What to do if startSimulation fails?
				retVal.emplace_back(new Spec::SIndividual(simulationLogPtr->info()));
			}
		}
	}
	return retVal;
}

Spec::Generation MotionGenerator::genesisRandom(std::size_t numIndividuals) {
	Spec::Generation retVal;

	std::vector<double> time(motionParameters.simSamples);
	auto timeGenerator = [this, t = motionParameters.simStart]() mutable {
		return (t += motionParameters.simStep);
	};
	std::generate(time.begin(), time.end(), timeGenerator);

	auto generateRandomVector = [&](std::pair<double, double> limits) -> std::vector<double> {
		std::vector<double> retVal(motionParameters.simSamples);
		auto vectorGenerator = [&]() {
			return DEvA::RandomNumberGenerator::get()->getRealBetween<double>(limits.first, limits.second);
		};
		std::generate(retVal.begin(), retVal.end(), vectorGenerator);
		return retVal;
	};

	for (size_t n(0); n != numIndividuals; ++n) {
		SimulationInfo simInfo{.generation = 0, .identifier = n};
		auto simDataPtr = database.createSimulation(simInfo);
		simDataPtr->time = time;
		simDataPtr->params.emplace("simStart", motionParameters.simStart);
		simDataPtr->params.emplace("simStop", motionParameters.simStop());
		simDataPtr->params.emplace("simStep", motionParameters.simStep);
		simDataPtr->params.emplace("simSamples", static_cast<double>(motionParameters.simSamples));
		for (auto & jointName : motionParameters.jointNames) {
			auto & jointLimits = motionParameters.jointLimits.at(jointName);
			auto randomVector = generateRandomVector(jointLimits);
			simDataPtr->torque.emplace(std::make_pair(jointName, randomVector));
		}
		SimulationLogPtr simulationLogPtr = database.getSimulationLog(simInfo);
		MGEA::ErrorCode startError = database.startSimulation(simulationLogPtr->info());
		// TODO: What to do if startSimulation fails?
		retVal.emplace_back(new Spec::SIndividual(simulationLogPtr->info()));
		//spdlog::info("Individual{} created", simInfo);
	}
	return retVal;
}

Spec::MaybePhenotypeProxy MotionGenerator::transform(Spec::GenotypeProxy genPx) {
	auto simLogPtr = database.getSimulationLog(genPx);
	if (simLogPtr->outputExists()) {
		return genPx;
	}
	while (true) {
		auto maybeSimulationDataPtr = database.getSimulationResult(simLogPtr->info());
		if (std::unexpected(MGEA::ErrorCode::SimulationError) == maybeSimulationDataPtr) {
			return std::unexpected(DEvA::ErrorCode::InvalidTransform);
		}
		if (maybeSimulationDataPtr and maybeSimulationDataPtr.value()) {
			return genPx;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::unreachable();
}

Spec::Fitness MotionGenerator::evaluate(Spec::GenotypeProxy genPx) {
	auto & timer = DTimer::simple("evaluate()").newSample().begin();
	auto simLogPtr = database.getSimulationLog(genPx);
	if (simLogPtr->fitnessExists()) {
		return simLogPtr->data()->fitness;
	}
	SimulationDataPtr simDataPtr = simLogPtr->data();
	
	if (!simDataPtr->outputs.contains("ankleHeight")) {
		spdlog::error("There is no position named \"ankleHeight\" in simulation output {}", genPx);
		return 0.0;
	}
	double timeStep = motionParameters.simStep;
	auto & ankleHeight = simDataPtr->outputs.at("ankleHeight");
	double ankleHeightSum = std::accumulate(ankleHeight.begin(), ankleHeight.end(), 0.0);
	double fitness = ankleHeightSum * timeStep;
	simLogPtr->data()->fitness = fitness;
	database.setSimulationFitness(simLogPtr->info(), fitness);
	//spdlog::info("Individual{} evaluated to fitness value {}", genPx, fitness);
	timer.end();
	return fitness;
}

template <std::size_t N, std::size_t M>
Spec::IndividualPtrs MotionGenerator::parentSelection(Spec::IndividualPtrs iptrs) {
	Spec::IndividualPtrs parents = DEvA::StandardParentSelectors<Spec>::bestNofM<N, M>(iptrs);
	for (auto it(parents.begin()); it != parents.end(); ++it) {
		auto & parent = *it;
		//spdlog::info("Selected {} as parent. It has fitness {}", parent->genotypeProxy, parent->fitness);
	}
	return parents;
}

bool MotionGenerator::convergenceCheck(Spec::Fitness f) {
	return f > 1.5 * motionParameters.simStop();
}

Spec::GenotypeProxies MotionGenerator::computeVariation(std::function<SimulationDataPtrs(MotionParameters const &, SimulationDataPtrs)> varFunc, Spec::GenotypeProxies parentProxies) {
	auto & timer = DTimer::simple("computeVariation()").newSample().begin();
	std::list<SimulationDataPtr> parentData;
	for (auto & parentProxy : parentProxies) {
		SimulationLogPtr parentLogPtr = database.getSimulationLog(parentProxy);
		SimulationDataPtr parentDataPtr = parentLogPtr->data();
		parentData.push_back(parentDataPtr);
	}

	auto children = varFunc(motionParameters, parentData);

	Spec::GenotypeProxies childProxies;
	for (auto & child : children) {
		SimulationInfo childInfo{.generation = currentGeneration, .identifier = database.nextId()};
		auto createResult = database.createSimulation(childInfo);
		// TODO Check if we could successfully create a new input?
		SimulationLogPtr childLogPtr = database.getSimulationLog(childInfo);
		updateSimulationDataPtr({.source = child, .target = childLogPtr->data()});
		MGEA::ErrorCode startError = database.startSimulation(childLogPtr->info());
		// TODO Check if start was successful?
		childProxies.push_back(childInfo);
	}
	timer.end();
	return childProxies;
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
		spdlog::info("(Min, mean, max) simulation times were: ({}s, {}s, {}s)", *minmax.first, mean, *minmax.second);
		spdlog::info("Total simulation time was: {}", total);
	}
	timer.end();
}
