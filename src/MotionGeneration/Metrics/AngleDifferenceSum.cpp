// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Metrics/Metrics.h"

#include <cmath>
#include <numbers>
#include <vector>

namespace MGEA {
	std::any angleDifferenceSum(DEvA::ParameterMap parameterMap, Spec::IndividualPtr iptr) {
		auto & simDataPtr(*iptr->maybePhenotype);
		VectorMap const & angles(simDataPtr->angles);

		OrderedVector anglesVector(orderedVectorFromVectorMap(angles));
		auto angleExtents(getExtents(anglesVector));
		if (angleExtents.empty() or 0 == angleExtents[0]) {
			return {};
		}

		double const simStep(iptr->genotype->params.at("simStep"));
		double const simStop(iptr->genotype->params.at("simStop"));
		OrderedVector differencesVector{};
		for (auto & angleVector : anglesVector) {
			DataVector differenceVector{};
			std::transform(angleVector.begin(), angleVector.end(), std::back_inserter(differenceVector), [&](auto const & angle) {
				return std::abs(angle - angleVector[0]);
			});
			differencesVector.emplace_back(differenceVector);
		}
		
		DataVector sumsOfAbsoluteDifferences{};
		for (auto & differenceVector : differencesVector) {
			double difference(std::accumulate(differenceVector.begin(), differenceVector.end(), 0.0));
			sumsOfAbsoluteDifferences.emplace_back(difference);
		}

		double sumOfAbsoluteDifferences(std::accumulate(sumsOfAbsoluteDifferences.begin(), sumsOfAbsoluteDifferences.end(), 0.0));
		double averageSumOfAbsoluteDifferences(sumOfAbsoluteDifferences * simStep / simStop);
		return averageSumOfAbsoluteDifferences * 360.0 / (2.0 * std::numbers::pi);
	}
}