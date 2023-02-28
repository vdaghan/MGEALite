// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Metrics/Metrics.h"

#include <cmath>
#include <vector>

namespace MGEA {
	double angleDifferenceSumLinearWeighted(Spec::IndividualPtr iptr) {
		auto & simDataPtr(*iptr->maybePhenotype);
		VectorMap const & angles(simDataPtr->angles);

		OrderedVector anglesVector(orderedVectorFromVectorMap(angles));
		auto angleExtents(getExtents(anglesVector));
		if (angleExtents.empty() or 0 == angleExtents[0]) {
			return {};
		}

		OrderedVector differencesVector{};
		for (auto & angleVector : anglesVector) {
			DataVector differenceVector{};
			std::size_t numSteps(256);
			for (std::size_t i(0); i != numSteps; ++i) {
				double value(std::abs(angleVector[i] - angleVector[0]));
				double scale(1.0 - 0.9 * static_cast<double>(i) / static_cast<double>(numSteps));
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
	}
}