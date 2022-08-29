// TODO: Read mapping between json tags and vector indices from a json file
// TODO: Read json input files

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "mgealite_version.h"
#include "Database.h"
#include "SimulationData.h"

#include "EvolutionaryAlgorithm.h"
#include "Specialisation.h"

int main() {
	std::cout << "MGEALite version: " << getMGEALiteVersion() << std::endl;
	Database db("./data");

	using Genotype = std::vector<std::vector<double>>;
	using Phenotype = std::vector<std::vector<double>>;
	using Fitness = double;

	using Spec = DEvA::Specialisation<Genotype, Phenotype, Fitness>;
	DEvA::EvolutionaryAlgorithm<Spec> ea;

	Spec::FEvaluate fevaluate = [](Spec::PhenotypePtr pptr) -> Spec::Fitness {
		double fitness(0.0);
		return fitness;
	};

	//SimulationData simulationInput = importSimulationData("./data/1_in.json");

	//std::vector<double> time;
	//for (double t(0.0); t < 4.096; t += 0.001) {
	//	time.push_back(t);
	//}
	//auto generateRandomVector = [](std::size_t vectorSize, double min, double max) -> std::vector<double> {
	//	std::default_random_engine randGen;
	//	std::uniform_real_distribution<double> distribution(min, max);
	//	std::vector<double> retVal;
	//	for (std::size_t i(0); i != vectorSize; ++i) {
	//		retVal.push_back(distribution(randGen));
	//	}
	//	return retVal;
	//};
	//SimulationData simulationInput;
	//simulationInput.time = time;
	//simulationInput.torque.emplace(std::make_pair("wrist", generateRandomVector(4096, -50, 50)));
	//simulationInput.torque.emplace(std::make_pair("shoulder", generateRandomVector(4096, -200, 200)));
	//simulationInput.torque.emplace(std::make_pair("hip", generateRandomVector(4096, -500, 500)));

	//exportSimulationData(simulationInput, "./data/2_in.json");

	return 0;
}
