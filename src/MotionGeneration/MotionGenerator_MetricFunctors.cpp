// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/MotionGenerator.h"

#include "MotionGeneration/Metrics/Metrics.h"

void MotionGenerator::createMetricFunctors() {
	auto computeBalanceLambda = [&](Spec::IndividualPtr iptr) {
		auto & simDataPtr(*iptr->maybePhenotype);

		auto & comX = simDataPtr->outputs.at("centerOfMassX");
		auto & palmX = simDataPtr->outputs.at("palmX");
		double balance = std::inner_product(comX.begin(), comX.end(), palmX.begin(), 0.0, std::plus<>(), [](auto const& cX, auto const& pX) {
			return std::abs(cX - pX);
		});
		double timeStep = motionParameters.simStep;
		balance = balance * timeStep / motionParameters.simStop();
		return balance;
	};
	auto balanceComparisonLambda = [](std::any const & lhs, std::any const & rhs) {
		return std::any_cast<double>(lhs) < std::any_cast<double>(rhs);
	};
	ea.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("computeBalance", computeBalanceLambda));

	auto computeFitnessLambda = [&](Spec::IndividualPtr iptr) {
		auto & simDataPtr(*iptr->maybePhenotype);
		double timeStep = motionParameters.simStep;
		auto absLambda = [](double prev, double next) {
			return prev + std::abs(next);
		};
		auto & fingertipZ = simDataPtr->outputs.at("fingertipZ");
		double fingertipZSum = std::accumulate(fingertipZ.begin(), fingertipZ.end(), 0.0);
		double maxFingertipZ = *std::max_element(fingertipZ.begin(), fingertipZ.end());
		auto & palmZ = simDataPtr->outputs.at("palmZ");
		double palmZSum = std::accumulate(palmZ.begin(), palmZ.end(), 0.0);
		double maxPalmZ = *std::max_element(palmZ.begin(), palmZ.end());
		auto & comZ = simDataPtr->outputs.at("centerOfMassZ");
		double comZSum = std::accumulate(comZ.begin(), comZ.end(), 0.0, absLambda);

		double fitness;
		//if (fingertipZSum <= 0.0 and palmZSum <= 0.0) {
		if (maxFingertipZ <= 0.001 and maxPalmZ <= 0.001) {
			fitness = comZSum * timeStep / motionParameters.simStop();
		} else {
			fitness = 0.0;
			if (maxFingertipZ > 0.001) {
				fitness -= maxFingertipZ;
			}
			if (maxPalmZ > 0.001) {
				fitness -= maxPalmZ;
			}
		}

		return fitness;
	};
	ea.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("computeFitness", computeFitnessLambda));

	ea.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("angularVelocitySign", &MGEA::angularVelocitySign));
	ea.metricFunctors.equivalences.emplace(std::pair("angularVelocitySignEquivalence", &MGEA::angularVelocitySignEquivalent));
	ea.metricFunctors.metricToJSONObjectFunctions.emplace(std::pair("orderedVector", &MGEA::orderedVectorConversion));
	//DEvA::MetricFunctor<Spec> angularVelocitySignMetricFunctor{
	//	.name = "angularVelocitySign",
	//	.computeFromIndividualPtrFunction = &MGEA::angularVelocitySign,
	//	.equivalentToFunction = &MGEA::angularVelocitySignEquivalent
	//};
	//angularVelocitySignMetricFunctor.constructDefaultJSONConverter<MGEA::OrderedVector>();
	//ea.registerMetricFunctor(angularVelocitySignMetricFunctor);
}
