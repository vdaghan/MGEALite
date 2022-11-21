#include "MotionGeneration/MotionGenerator.h"

void MotionGenerator::createMetricFunctors() {
	auto balanceEvaluateLambda = [&](Spec::IndividualPtr iptr) {
		auto & simDataPtr(iptr->genotype);

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
	DEvA::MetricFunctor<Spec> balanceMetricFunctor{
		.name = "balance",
		.computeFromIndividualPtrFunction = balanceEvaluateLambda,
		.betterThanFunction = balanceComparisonLambda
	};
	ea.registerMetricFunctor(balanceMetricFunctor);

	auto fitnessEvaluateLambda = [&](Spec::IndividualPtr iptr) {
		auto & simDataPtr(iptr->genotype);
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
		if (fingertipZSum <= 0.0 and palmZSum <= 0.0) {
			fitness = comZSum * timeStep / motionParameters.simStop();
		}
		else {
			fitness = 0.0;
			if (fingertipZSum > 0.0) {
				fitness -= maxFingertipZ;
			}
			if (palmZSum > 0.0) {
				fitness -= maxPalmZ;
			}
		}

		return fitness;
	};
	auto fitnessComparisonLambda = [](std::any lhs, std::any rhs) {
		double lhsFitness(std::any_cast<double>(lhs));
		double rhsFitness(std::any_cast<double>(rhs));
		return lhsFitness > rhsFitness;
	};
	DEvA::MetricFunctor<Spec> fitnessMetricFunctor{
		.name = "fitness",
		.computeFromIndividualPtrFunction = fitnessEvaluateLambda,
		.betterThanFunction = fitnessComparisonLambda
	};
	ea.registerMetricFunctor(fitnessMetricFunctor);

	auto angularVelocitySignEvaluateLambda = [&](Spec::IndividualPtr iptr) {
		auto & simDataPtr(iptr->genotype);
		return MGEA::computeAngularVelocitySign(simDataPtr->angles);
	};
	auto angularVelocitySignComparisonLambda = [](std::any lhs, std::any rhs) {
		MGEA::OrderedVector lhsAngularVelocitySign(std::any_cast<MGEA::OrderedVector>(lhs));
		MGEA::OrderedVector rhsAngularVelocitySign(std::any_cast<MGEA::OrderedVector>(rhs));
		std::size_t minVectorSize(std::min(lhsAngularVelocitySign.size(), rhsAngularVelocitySign.size()));
		for (std::size_t i(0); i != minVectorSize; ++i) {
			std::size_t numAngles(lhsAngularVelocitySign.at(i).size());
			if (numAngles != rhsAngularVelocitySign.at(i).size()) {
				throw;
			}
			for (std::size_t j(0); j != numAngles; ++j) {
				bool equalValue(lhsAngularVelocitySign.at(i).at(j) == rhsAngularVelocitySign.at(i).at(j));
				bool lhsIsZero(lhsAngularVelocitySign.at(i).at(j) == 0);
				bool rhsIsZero(rhsAngularVelocitySign.at(i).at(j) == 0);
				if (not (equalValue or lhsIsZero or rhsIsZero)) {
					return false;
				}
			}
		}
		return true;
	};
	DEvA::MetricFunctor<Spec> angularVelocitySignMetricFunctor{
		.name = "angularVelocitySign",
		.computeFromIndividualPtrFunction = angularVelocitySignEvaluateLambda,
		.equivalentToFunction = angularVelocitySignComparisonLambda
	};
	ea.registerMetricFunctor(angularVelocitySignMetricFunctor);
}
