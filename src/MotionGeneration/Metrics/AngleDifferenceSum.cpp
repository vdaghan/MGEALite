// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Metrics/Metrics.h"

#include <cmath>
#include <vector>

namespace MGEA {
	double angleDifferenceSum(Spec::IndividualPtr iptr) {
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
			std::transform(angleVector.begin(), angleVector.end(), differenceVector.begin(), [&](auto const& angle) {
				return std::abs(angle - angleVector[0]);
			});
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