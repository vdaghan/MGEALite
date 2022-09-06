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
};

void MotionGenerator::epoch() {
	auto currentGeneration = database.getCurrentGeneration();
	spdlog::info("Epoch {} started.", *currentGeneration);
	auto stepResult = ea.epoch();
	spdlog::info("Epoch {} ended.", *currentGeneration);
};

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

	database.createNewGeneration();
	for (size_t n(0); n != 500; ++n) {
		auto simLogPtr = database.createSimulationInThisGeneration();
		spdlog::info("Created Individual({}, {})", simLogPtr->generation(), simLogPtr->identifier());
		SimulationDataPtr simDataPtr = std::make_shared<SimulationData>();
		simDataPtr->time = time;
		simDataPtr->params.emplace("simStart", 0.0);
		simDataPtr->params.emplace("simStop", 4.095);
		simDataPtr->params.emplace("simStep", 0.001);
		simDataPtr->params.emplace("simSamples", 4096);
		simDataPtr->torque.emplace(std::make_pair("wrist", generateRandomVector()));
		simDataPtr->torque.emplace(std::make_pair("shoulder", generateRandomVector()));
		simDataPtr->torque.emplace(std::make_pair("hip", generateRandomVector()));
		simLogPtr->createInput(simDataPtr);
		retVal.emplace_back(new Spec::SIndividual(simLogPtr->info()));
		spdlog::info("Individual({}, {}) created", simLogPtr->generation(), simLogPtr->identifier());
	}
	return retVal;
}

Spec::PhenotypeProxy MotionGenerator::transform(Spec::GenotypeProxy genPx) {
	auto simLogPtr = database.getSimulation(genPx);
	while (!simLogPtr->outputExists()) {
		//spdlog::info("Waiting for output of Individual({}, {})", genPx.generation, genPx.identifier);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return genPx;
}

Spec::Fitness MotionGenerator::evaluate(Spec::GenotypeProxy genPx) {
	spdlog::info("Evaluating Individual({}, {})", genPx.generation, genPx.identifier);
	auto simLogPtr = database.getSimulation(genPx);
	SimulationDataPtr simDataPtr = simLogPtr->loadOutput();
	
	if (!simDataPtr->outputs.contains("ankleHeight")) {
		spdlog::error("There is no position named \"ankleHeight\" in simulation output ({}, {})", genPx.generation, genPx.identifier);
		return 0.0;
	}
	double timeStep = 0.001;
	auto & ankleHeight = simDataPtr->outputs.at("ankleHeight");
	double ankleHeightSum = std::accumulate(ankleHeight.begin(), ankleHeight.end(), 0.0);
	double fitness = ankleHeightSum * timeStep;
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
	SimulationLogPtr simulation1LogPtr = database.getSimulation(simulation1Info);
	SimulationLogPtr simulation2LogPtr = database.getSimulation(simulation2Info);
	SimulationDataPtr simulation1DataPtr = simulation1LogPtr->loadInput();
	SimulationDataPtr simulation2DataPtr = simulation2LogPtr->loadInput();

	auto children = cutAndCrossfillSingle({simulation1DataPtr, simulation2DataPtr});

	SimulationLogPtr child1LogPtr = database.createSimulationInThisGeneration();
	SimulationLogPtr child2LogPtr = database.createSimulationInThisGeneration();
	child1LogPtr->createInput(children.front());
	child2LogPtr->createInput(children.back());
	// TODO Check if we could successfully create a new input?
	return {child1LogPtr->info(), child2LogPtr->info()};
}
