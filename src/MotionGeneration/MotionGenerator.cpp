#include "MotionGeneration/MotionGenerator.h"
#include "MotionGeneration/Variations/CutAndCrossfillSingle.h"

#include <algorithm>
#include <functional>

#include "spdlog/spdlog.h"

MotionGenerator::MotionGenerator(std::string folder) : database(folder) {
	ea.setGenesisFunction(std::bind_front(&MotionGenerator::genesis, this));
	ea.setTransformFunction(std::bind_front(&MotionGenerator::transform, this));
	ea.setEvaluationFunction(std::bind_front(&MotionGenerator::evaluate, this));
	Spec::SVariationFunctor variationFunctor;
	variationFunctor.setParentSelectionFunction(std::bind_front(&MotionGenerator::parentSelection, this));
	variationFunctor.setVariationFunction(std::bind_front(&MotionGenerator::cutAndCrossfillVariation, this));
	variationFunctor.setProbability(1.0);
	variationFunctor.setRemoveParentFromMatingPool(false);
	ea.addVariationFunctor(variationFunctor);
	ea.setSurvivorSelectionFunction(std::bind_front(&MotionGenerator::survivorSelection, this));
	ea.setConvergenceCheckFunction(std::bind_front(&MotionGenerator::convergenceCheck, this));

	ea.setOnEpochStartCallback(std::bind_front(&MotionGenerator::onEpochStart, this));
	ea.setOnEpochEndCallback(std::bind_front(&MotionGenerator::onEpochEnd, this));
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

void MotionGenerator::search(std::size_t n) {
	auto stepResult = ea.search(n);
}

Spec::Generation MotionGenerator::genesis() {
	Spec::Generation retVal;
	std::random_device randDev;
	std::default_random_engine randGen(randDev());
	std::uniform_int_distribution<std::size_t> distributionVal(0, 4095);
	std::uniform_real_distribution<double> randRealDist(0, 1);

	std::vector<double> time(4096);
	std::generate(time.begin(), time.end(), [t = 0.0]() mutable { return (t += 0.001); });

	auto generateRandomVector = [&]() -> std::vector<double> {
		std::vector<double> retVal(4096);
		std::generate(retVal.begin(), retVal.end(), [&](){ return randRealDist(randGen); });
		return retVal;
	};

	for (size_t n(0); n != 6; ++n) {
		SimulationInfo simInfo{.generation = 0, .identifier = n};
		auto simDataPtr = database.createSimulation(simInfo);
		simDataPtr->time = time;
		simDataPtr->params.emplace("simStart", 0.0);
		simDataPtr->params.emplace("simStop", 4.095);
		simDataPtr->params.emplace("simStep", 0.001);
		simDataPtr->params.emplace("simSamples", 4096);
		simDataPtr->torque.emplace(std::make_pair("wrist", generateRandomVector()));
		simDataPtr->torque.emplace(std::make_pair("shoulder", generateRandomVector()));
		simDataPtr->torque.emplace(std::make_pair("hip", generateRandomVector()));
		SimulationLogPtr simulationLogPtr = database.getSimulationLog(simInfo);
		simulationLogPtr->updateStatus(SimulationStatus::PendingSimulation);
		bool startSuccessful = database.startSimulation(simulationLogPtr->info());
		// TODO: What to do if startSimulation fails?
		retVal.emplace_back(new Spec::SIndividual(simulationLogPtr->info()));
		spdlog::info("Individual({}, {}) created", simInfo.generation, simInfo.identifier);
	}
	return retVal;
}

Spec::PhenotypeProxy MotionGenerator::transform(Spec::GenotypeProxy genPx) {
	auto simLogPtr = database.getSimulationLog(genPx);
	while (!database.getSimulationResult(simLogPtr->info())) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return genPx;
}

Spec::Fitness MotionGenerator::evaluate(Spec::GenotypeProxy genPx) {
	auto simLogPtr = database.getSimulationLog(genPx);
	if (simLogPtr->fitnessExists()) {
		return simLogPtr->data()->fitness;
	}
	spdlog::info("Evaluating Individual({}, {})", genPx.generation, genPx.identifier);
	SimulationDataPtr simDataPtr = simLogPtr->data();
	
	if (!simDataPtr->outputs.contains("ankleHeight")) {
		spdlog::error("There is no position named \"ankleHeight\" in simulation output ({}, {})", genPx.generation, genPx.identifier);
		return 0.0;
	}
	double timeStep = 0.001;
	auto & ankleHeight = simDataPtr->outputs.at("ankleHeight");
	double ankleHeightSum = std::accumulate(ankleHeight.begin(), ankleHeight.end(), 0.0);
	double fitness = ankleHeightSum * timeStep;
	simLogPtr->data()->fitness = fitness;
	simLogPtr->updateStatus(SimulationStatus::Computed);
	spdlog::info("Individual({}, {}) evaluated to fitness value {}", genPx.generation, genPx.identifier, fitness);
	return fitness;
}

Spec::IndividualPtrs MotionGenerator::parentSelection(Spec::IndividualPtrs iptrs) {
	Spec::IndividualPtrs parents = DEvA::StandardParentSelectors<Spec>::bestNofM<2, 5>(iptrs);
	for (auto it(parents.begin()); it != parents.end(); ++it) {
		auto & parent = *it;
		spdlog::info("Selected parent ({}, {}) with fitness {}", parent->genotypeProxy.generation, parent->genotypeProxy.identifier, parent->fitness);
	}
	return parents;
}

void MotionGenerator::survivorSelection(Spec::IndividualPtrs & iptrs) {
	DEvA::StandardSurvivorSelectors<Spec>::clamp<100>(iptrs);
}

bool MotionGenerator::convergenceCheck(Spec::Fitness f) {
	return f > 1.0;
}

Spec::GenotypeProxies MotionGenerator::cutAndCrossfillVariation(Spec::GenotypeProxies gpxs) {
	Spec::GenotypeProxy simulation1Info= gpxs.front();
	Spec::GenotypeProxy simulation2Info = gpxs.back();
	SimulationLogPtr simulation1LogPtr = database.getSimulationLog(simulation1Info);
	SimulationLogPtr simulation2LogPtr = database.getSimulationLog(simulation2Info);
	SimulationDataPtr simulation1DataPtr = simulation1LogPtr->data();
	SimulationDataPtr simulation2DataPtr = simulation2LogPtr->data();

	auto children = cutAndCrossfillSingle({simulation1DataPtr, simulation2DataPtr});

	SimulationInfo sim1Info{.generation = currentGeneration, .identifier = database.nextId()};
	database.createSimulation(sim1Info);
	SimulationLogPtr child1LogPtr = database.getSimulationLog(sim1Info);

	SimulationInfo sim2Info{.generation = currentGeneration, .identifier = database.nextId()};
	database.createSimulation(sim2Info);
	SimulationLogPtr child2LogPtr = database.getSimulationLog(sim2Info);

	*child1LogPtr->data() = *children.front();
	*child2LogPtr->data() = *children.back();
	// TODO Check if we could successfully create a new input?
	return {child1LogPtr->info(), child2LogPtr->info()};
}

void MotionGenerator::onEpochStart(std::size_t generation) {
	currentGeneration = generation;
	spdlog::info("Epoch {} started.", generation);
}

void MotionGenerator::onEpochEnd(std::size_t generation) {
	spdlog::info("Epoch {} ended.", generation);
}
