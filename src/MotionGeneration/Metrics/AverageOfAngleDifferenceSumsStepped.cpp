// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Metrics/Metrics.h"

#include <cmath>
#include <numbers>
#include <vector>

namespace MGEA {
	std::any averageOfAngleDifferenceSumsStepped(DEvA::ParameterMap parameters, Spec::IndividualPtr iptr) {
		std::size_t maxSteps(parameters.at("maxSteps").get<std::size_t>());
		double stepThreshold(parameters.at("stepThreshold").get<double>());

		auto & simDataPtr(*iptr->maybePhenotype);
		VectorMap const & angles(simDataPtr->angles);

		OrderedVector anglesVector(orderedVectorFromVectorMap(angles));
		auto angleExtents(getExtents(anglesVector));
		if (angleExtents.empty() or 0 == angleExtents[0]) {
			return {};
		}

		std::size_t const numSteps(iptr->genotype->time.size());
		std::size_t const numJoints(anglesVector.size());
		double const simStep(iptr->genotype->params.at("simStep"));
		double const simStop(iptr->genotype->params.at("simStop"));
		double const normalisationCoefficient(static_cast<double>(numJoints) * simStep / simStop);
		auto computeFitnessUsingStage = [&](double stage) {
			OrderedVector differencesVector{};
			for (auto & angleVector : anglesVector) {
				DataVector differenceVector{};
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

			double sumOfAbsoluteDifferences(std::accumulate(sumsOfAbsoluteDifferences.begin(), sumsOfAbsoluteDifferences.end(), 0.0));
			double averageSumOfAbsoluteDifferences(sumOfAbsoluteDifferences * normalisationCoefficient);
			return averageSumOfAbsoluteDifferences * 360.0 / (2.0 * std::numbers::pi);
		};

		double fitness(0.0);
		std::size_t stage(0);
		while (fitness < stepThreshold and stage < maxSteps) {
			fitness = computeFitnessUsingStage(static_cast<double>(stage) / static_cast<double>(numSteps));
			++stage;
		}

		return SteppedDouble{ stage, fitness };
	}
}