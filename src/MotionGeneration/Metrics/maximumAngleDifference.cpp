// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Metrics/Metrics.h"

#include <cmath>
#include <numbers>
#include <vector>

namespace MGEA {
	std::any maximumAngleDifference(DEvA::ParameterMap parameters, Spec::IndividualPtr iptr) {
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
		OrderedVector differencesVector{};
		for (auto & angleVector : anglesVector) {
			DataVector differenceVector{};
			for (std::size_t i(0); i != numSteps; ++i) {
				double value(std::abs(angleVector[i] - angleVector[0]));
				double x(static_cast<double>(i) / static_cast<double>(numSteps));
				differenceVector.emplace_back(value);
			}
			differencesVector.emplace_back(differenceVector);
		}

		DataVector maximumsVector{};
		for (auto & differenceVector : differencesVector) {
			auto max(std::max_element(differenceVector.begin(), differenceVector.end()));
			maximumsVector.emplace_back(*max);
		}
		double maximum(*std::max_element(maximumsVector.begin(), maximumsVector.end()));

		return maximum * 360.0 / (2.0 * std::numbers::pi);
	}
}
