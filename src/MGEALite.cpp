#include "EvolutionaryAlgorithm.h"
#include "Specialisation.h"

int main() {

	using Genotype = std::vector<size_t>;
	using Phenotype = std::vector<size_t>;
	using Fitness = int;

	using Spec = DEvA::Specialisation<Genotype, Phenotype, Fitness>;
	DEvA::EvolutionaryAlgorithm<Spec> ea;

	return 0;
}
