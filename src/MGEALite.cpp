// TODO: Read mapping between json tags and vector indices from a json file
// TODO: Read json input files

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "EvolutionaryAlgorithm.h"
#include "Specialisation.h"

using JSON = nlohmann::ordered_json;

struct SimulationInput {
	std::vector<double> time;
	std::map<std::string, double> params;
	std::map<std::string, std::vector<double>> torque;
};

void to_json(JSON& j, const SimulationInput& sI) {
	j = JSON{ {"time", sI.time}, {"params", sI.params}, {"torque", sI.torque} };
}

void from_json(const JSON& j, SimulationInput& sI) {
	j.at("time").get_to(sI.time);
	j.at("params").get_to(sI.params);
	j.at("torque").get_to(sI.torque);
}

int main() {

	using Genotype = std::vector<std::vector<double>>;
	using Phenotype = std::vector<std::vector<double>>;
	using Fitness = double;

	using Spec = DEvA::Specialisation<Genotype, Phenotype, Fitness>;
	DEvA::EvolutionaryAlgorithm<Spec> ea;

	Spec::FEvaluate fevaluate = [](Spec::PhenotypePtr pptr) -> Spec::Fitness {
		double fitness(0.0);
		return fitness;
	};

	std::ifstream f("./data/1_in.json");
	nlohmann::json data = nlohmann::json::parse(f);

	std::vector<double> time;
	for (double t(0.0); t < 4.096; t += 0.001) {
		time.push_back(t);
	}
	auto generateRandomVector = [](std::size_t vectorSize, double min, double max) -> std::vector<double> {
		std::default_random_engine randGen;
		std::uniform_real_distribution<double> distribution(min, max);
		std::vector<double> retVal;
		for (std::size_t i(0); i != vectorSize; ++i) {
			retVal.push_back(distribution(randGen));
		}
		return retVal;
	};
	SimulationInput simulationInput;
	simulationInput.time = time;
	simulationInput.torque.emplace(std::make_pair("wrist", generateRandomVector(4096, -50, 50)));
	simulationInput.torque.emplace(std::make_pair("shoulder", generateRandomVector(4096, -200, 200)));
	simulationInput.torque.emplace(std::make_pair("hip", generateRandomVector(4096, -500, 500)));

	JSON j = simulationInput;
	std::ofstream o("./data/2_in.json");
	o << std::setw(4) << j << std::endl;

	return 0;
}
