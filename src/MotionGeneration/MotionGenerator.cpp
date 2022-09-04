#include "MotionGeneration/MotionGenerator.h"

#include <algorithm>
#include <functional>

#include "spdlog/spdlog.h"

MotionGenerator::MotionGenerator(std::string folder) : database(folder) {
	ea.setGenesisFunction(std::bind_front(&MotionGenerator::genesis, this));
	ea.setTransformFunction(std::bind_front(&MotionGenerator::transform, this));
	ea.setEvaluationFunction(std::bind_front(&MotionGenerator::evaluate, this));
	ea.setParentSelectionFunction(std::bind_front(&MotionGenerator::parentSelection, this));
	ea.setVariationFunction(std::bind_front(&MotionGenerator::variation, this));
	ea.setSurvivorSelectionFunction(std::bind_front(&MotionGenerator::survivorSelection, this));
	ea.setConvergenceCheckFunction(std::bind_front(&MotionGenerator::convergenceCheck, this));
};

void MotionGenerator::start() {
	auto stepResult = ea.epoch();
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

	for (size_t n(0); n != 100; ++n) {
		spdlog::info("Creating Individual({}, {})", 0, n);
		SimulationInfo simulationInfo{.generation = 0, .identifier = n};
		auto simLogPtr = database.createSimulation(simulationInfo);
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
		retVal.emplace_back(new Spec::SIndividual(simulationInfo));
		spdlog::info("Individual({}, {}) created", 0, n);
	}
	return retVal;
}

Spec::PhenotypeProxy MotionGenerator::transform(Spec::GenotypeProxy genPx) {
	auto simLogPtr = database.getSimulation(genPx);
	while (!simLogPtr->outputExists()) {
		spdlog::info("Waiting for output of Individual({}, {})", genPx.generation, genPx.identifier);
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

Spec::IndividualPtrs MotionGenerator::parentSelection(Spec::IndividualPtrs) {
	return {};
}

Spec::GenotypeProxies MotionGenerator::variation(Spec::GenotypeProxies) {
	return {};
}

void MotionGenerator::survivorSelection(Spec::IndividualPtrs &) {

}

bool MotionGenerator::convergenceCheck(Spec::Fitness) {
	return true;
}
