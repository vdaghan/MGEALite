// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include "MGEA.h"

using Fitness = std::pair<std::size_t, double>;

Fitness computeFitness(Spec::IndividualPtr iptr) {
	using namespace MGEA;

	auto & simDataPtr(*iptr->maybePhenotype);
	VectorMap const & angles(simDataPtr->angles);

	OrderedVector anglesVector(orderedVectorFromVectorMap(angles));
	auto angleExtents(getExtents(anglesVector));
	if (angleExtents.empty() or 0 == angleExtents[0]) {
		return {};
	}

	auto computeFitnessUsingStage = [&](double stage) {
		OrderedVector differencesVector{};
		for (auto & angleVector : anglesVector) {
			DataVector differenceVector{};
			std::size_t numSteps(256);
			for (std::size_t i(0); i != numSteps; ++i) {
				double value(std::abs(angleVector[i] - angleVector[0]));
				double x(static_cast<double>(i) / static_cast<double>(numSteps));
				// See https://www.wolframalpha.com/input?i=plot+%281-tanh%2832%28x-0.1%29%29%29%2F2+for+x+between+0+and+1%2C+y++between+0+and+1
				double scale(0.5 * (1.0 - std::tanh(32.0 * (x - (stage + 0.1)))));
				differenceVector.emplace_back(value * scale);
			}
			differencesVector.emplace_back(differenceVector);
		}

		DataVector sumsOfAbsoluteDifferences{};
		for (auto & differenceVector : differencesVector) {
			double difference(std::accumulate(differenceVector.begin(), differenceVector.end(), 0.0));
			sumsOfAbsoluteDifferences.emplace_back(difference);
		}

		return std::accumulate(sumsOfAbsoluteDifferences.begin(), sumsOfAbsoluteDifferences.end(), 0.0);
	};

	double fitness(0.0);
	std::size_t stage(0);
	while (fitness < static_cast<double>(stage+1) and stage < 50) {
		fitness = computeFitnessUsingStage(static_cast<double>(stage) / 50.0);
		++stage;
	}

	return { stage, fitness };
}

bool fitnessBetterThan(std::any const & lhs, std::any const & rhs) {
	auto lhsPair(std::any_cast<Fitness>(lhs));
	auto rhsPair(std::any_cast<Fitness>(rhs));
	if (lhsPair.first != rhsPair.first) {
		return lhsPair.first > rhsPair.first;
	}
	return lhsPair.second < rhsPair.second;
}

bool fitnessEquivalence(std::any const & lhs, std::any const & rhs) {
	auto lhsPair(std::any_cast<Fitness>(lhs));
	auto rhsPair(std::any_cast<Fitness>(rhs));
	return (lhsPair.first == rhsPair.first) and (lhsPair.second == rhsPair.second);
}

JSON fitnessToJSON(std::any const & fitnessAsPair) {
	Fitness fitness(std::any_cast<Fitness>(fitnessAsPair));
	return fitness;
}

int main() {
	spdlog::info("DEvA version: {}", getDEvAVersion());
	spdlog::info("MGEALite version: {}", getMGEALiteVersion());

	MotionGenerator motionGenerator("./data", "./02_standstill.json");
	motionGenerator.metricFunctors.orderings.emplace(std::pair("fitnessBetterThan", fitnessBetterThan));
	motionGenerator.metricFunctors.equivalences.emplace(std::pair("fitnessEquivalence", fitnessEquivalence));
	motionGenerator.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("computeFitness", computeFitness));
	motionGenerator.metricFunctors.metricToJSONObjectFunctions.emplace(std::pair("fitnessToJSON", fitnessToJSON));

	auto compileResult = motionGenerator.compile();
	motionGenerator.lambda = 256;
	auto result = motionGenerator.search(50);

	return 0;
}
