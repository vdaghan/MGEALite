#include "MotionGeneration/MotionGenerator.h"

#include <functional>

MotionGenerator::MotionGenerator() {
	ea.setGenesisFunction(std::bind_front(&MotionGenerator::genesis, this));
	ea.setTransformFunction(std::bind_front(&MotionGenerator::transform, this));
	ea.setEvaluationFunction(std::bind_front(&MotionGenerator::evaluate, this));
	ea.setParentSelectionFunction(std::bind_front(&MotionGenerator::parentSelection, this));
	ea.setVariationFunction(std::bind_front(&MotionGenerator::variation, this));
	ea.setSurvivorSelectionFunction(std::bind_front(&MotionGenerator::survivorSelection, this));
	ea.setConvergenceCheckFunction(std::bind_front(&MotionGenerator::convergenceCheck, this));
};

Spec::Generation MotionGenerator::genesis() {
	return Spec::Generation();
}

Spec::PhenotypePtr MotionGenerator::transform(Spec::GenotypePtr) {
	return nullptr;
}

Spec::Fitness MotionGenerator::evaluate(Spec::GenotypePtr) {
	return 0.0;
}

Spec::IndividualPtrs MotionGenerator::parentSelection(Spec::IndividualPtrs) {
	return {};
}

Spec::GenotypePtrs MotionGenerator::variation(Spec::GenotypePtrs) {
	return {};
}

void MotionGenerator::survivorSelection(Spec::IndividualPtrs &) {

}

bool MotionGenerator::convergenceCheck(Spec::Fitness) {
	return true;
}
