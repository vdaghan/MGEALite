#include "MotionGeneration/MotionGenerator.h"
#include "MotionGeneration/Variations/CrossoverAll.h"
#include "MotionGeneration/Variations/CrossoverSingle.h"
#include "MotionGeneration/Variations/CutAndCrossfillAll.h"
#include "MotionGeneration/Variations/CutAndCrossfillSingle.h"
#include "MotionGeneration/Variations/SNV.h"
#include "Logging/SpdlogCommon.h"

#include <algorithm>
#include <functional>

MotionGenerator::MotionGenerator(std::string folder, MotionParameters mP) : database(folder), motionParameters(mP) {
	ea.setGenesisFunction(std::bind_front(&MotionGenerator::genesis, this));
	ea.setTransformFunction(std::bind_front(&MotionGenerator::transform, this));
	ea.setEvaluationFunction(std::bind_front(&MotionGenerator::evaluate, this));
	ea.setFitnessComparisonFunction([](Spec::Fitness lhs, Spec::Fitness rhs){ return lhs > rhs; });
	Spec::SVariationFunctor variationFunctorCrossoverAll;
	variationFunctorCrossoverAll.setParentSelectionFunction(std::bind_front(&MotionGenerator::parentSelection<2, 10>, this));
	variationFunctorCrossoverAll.setVariationFunction(std::bind_front(&MotionGenerator::computeVariation, this, &crossoverAll));
	variationFunctorCrossoverAll.setProbability(1.0);
	variationFunctorCrossoverAll.setRemoveParentFromMatingPool(true);
	ea.addVariationFunctor(variationFunctorCrossoverAll);
	Spec::SVariationFunctor variationFunctorCrossoverSingle;
	variationFunctorCrossoverSingle.setParentSelectionFunction(std::bind_front(&MotionGenerator::parentSelection<2, 10>, this));
	variationFunctorCrossoverSingle.setVariationFunction(std::bind_front(&MotionGenerator::computeVariation, this, &crossoverSingle));
	variationFunctorCrossoverSingle.setProbability(0.5);
	variationFunctorCrossoverSingle.setRemoveParentFromMatingPool(true);
	ea.addVariationFunctor(variationFunctorCrossoverSingle);
	Spec::SVariationFunctor variationFunctorCutAndCrossfillAll;
	variationFunctorCutAndCrossfillAll.setParentSelectionFunction(std::bind_front(&MotionGenerator::parentSelection<2, 10>, this));
	variationFunctorCutAndCrossfillAll.setVariationFunction(std::bind_front(&MotionGenerator::computeVariation, this, &cutAndCrossfillAll));
	variationFunctorCutAndCrossfillAll.setProbability(0.1);
	variationFunctorCutAndCrossfillAll.setRemoveParentFromMatingPool(true);
	ea.addVariationFunctor(variationFunctorCutAndCrossfillAll);
	Spec::SVariationFunctor variationFunctorCutAndCrossfillSingle;
	variationFunctorCutAndCrossfillSingle.setParentSelectionFunction(std::bind_front(&MotionGenerator::parentSelection<2, 10>, this));
	variationFunctorCutAndCrossfillSingle.setVariationFunction(std::bind_front(&MotionGenerator::computeVariation, this, &cutAndCrossfillSingle));
	variationFunctorCutAndCrossfillSingle.setProbability(0.1);
	variationFunctorCutAndCrossfillSingle.setRemoveParentFromMatingPool(true);
	ea.addVariationFunctor(variationFunctorCutAndCrossfillSingle);
	Spec::SVariationFunctor variationFunctorSNV;
	variationFunctorSNV.setParentSelectionFunction(std::bind_front(&MotionGenerator::parentSelection<1, 10>, this));
	variationFunctorSNV.setVariationFunction(std::bind_front(&MotionGenerator::computeVariation, this, &snv));
	variationFunctorSNV.setProbability(1.0);
	variationFunctorSNV.setRemoveParentFromMatingPool(true);
	ea.addVariationFunctor(variationFunctorSNV);
	ea.setSurvivorSelectionFunction(std::bind_front(&MotionGenerator::survivorSelection, this));
	ea.setConvergenceCheckFunction(std::bind_front(&MotionGenerator::convergenceCheck, this));

	ea.setOnEpochStartCallback(std::bind_front(&MotionGenerator::onEpochStart, this));
	ea.setOnEpochEndCallback(std::bind_front(&MotionGenerator::onEpochEnd, this));
	ea.setLambda(100);
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

Spec::Generation MotionGenerator::genesis() {
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

	for (size_t n(0); n != 100; ++n) {
		SimulationInfo simInfo{.generation = 0, .identifier = n};
		auto simDataPtr = database.createSimulation(simInfo);
		simDataPtr->time = time;
		simDataPtr->params.emplace("simStart", motionParameters.simStart);
		simDataPtr->params.emplace("simStop", motionParameters.simStop());
		simDataPtr->params.emplace("simStep", motionParameters.simStep);
		simDataPtr->params.emplace("simSamples", motionParameters.simSamples);
		for (auto & jointName : motionParameters.jointNames) {
			auto & jointLimits = motionParameters.jointLimits.at(jointName);
			auto randomVector = generateRandomVector(jointLimits);
			simDataPtr->torque.emplace(std::make_pair(jointName, randomVector));
		}
		SimulationLogPtr simulationLogPtr = database.getSimulationLog(simInfo);
		MGEA::ErrorCode startError = database.startSimulation(simulationLogPtr->info());
		// TODO: What to do if startSimulation fails?
		retVal.emplace_back(new Spec::SIndividual(simulationLogPtr->info()));
		spdlog::info("Individual{} created", simInfo);
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
	spdlog::info("Individual{} evaluated to fitness value {}", genPx, fitness);
	return fitness;
}

template <std::size_t N, std::size_t M>
Spec::IndividualPtrs MotionGenerator::parentSelection(Spec::IndividualPtrs iptrs) {
	Spec::IndividualPtrs parents = DEvA::StandardParentSelectors<Spec>::bestNofM<N, M>(iptrs);
	for (auto it(parents.begin()); it != parents.end(); ++it) {
		auto & parent = *it;
		spdlog::info("Selected {} as parent. It has fitness {}", parent->genotypeProxy, parent->fitness);
	}
	return parents;
}

void MotionGenerator::survivorSelection(Spec::IndividualPtrs & iptrs) {
	DEvA::StandardSurvivorSelectors<Spec>::clamp<100>(iptrs);
}

bool MotionGenerator::convergenceCheck(Spec::Fitness f) {
	return f > 1.0;
}

Spec::GenotypeProxies MotionGenerator::computeVariation(std::function<SimulationDataPtrs(SimulationDataPtrs)> varFunc, Spec::GenotypeProxies parentProxies) {
	std::list<SimulationDataPtr> parentData;
	for (auto & parentProxy : parentProxies) {
		SimulationLogPtr parentLogPtr = database.getSimulationLog(parentProxy);
		SimulationDataPtr parentDataPtr = parentLogPtr->data();
		parentData.push_back(parentDataPtr);
	}

	auto children = varFunc(parentData);

	Spec::GenotypeProxies childProxies;
	for (auto & child : children) {
		SimulationInfo childInfo{.generation = currentGeneration, .identifier = database.nextId()};
		database.createSimulation(childInfo);
		// TODO Check if we could successfully create a new input?
		SimulationLogPtr childLogPtr = database.getSimulationLog(childInfo);
		*childLogPtr->data() = *child;
		MGEA::ErrorCode startError = database.startSimulation(childLogPtr->info());
		// TODO Check if start was successful?
		childProxies.push_back(childInfo);
	}
	return childProxies;
}

void MotionGenerator::onEpochStart(std::size_t generation) {
	currentGeneration = generation;
	spdlog::info("Epoch {} started.", generation);
}

void MotionGenerator::onEpochEnd(std::size_t generation) {
	spdlog::info("Epoch {} ended.", generation);
}
