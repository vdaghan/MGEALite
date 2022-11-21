#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include <algorithm>

namespace MGEA {
	void cullPartiallyDominated(std::vector<std::string> metrics, Spec::IndividualPtrs & iptrs) {
		auto isPartiallyDominatedLambda = [&](auto & iptr) {
			for (auto & otherptr : iptrs) {
				if (otherptr->id == iptr->id) [[unlikely]] {
					continue;
				}
				std::size_t numDominatingMetrics(0);
				for (auto & metricName : metrics) {
					bool otherIsBetter = (otherptr->metricMap.at(metricName) < iptr->metricMap.at(metricName));
					if (otherIsBetter) {
						++numDominatingMetrics;
					}
				}
				if (numDominatingMetrics + 1 == metrics.size()) {
					return true;
				}
			}
			return false;
		};

		std::list<Spec::IndividualPtr> retVal{};
		for (auto & iptr : iptrs) {
			if (!isPartiallyDominatedLambda(iptr)) {
				retVal.push_back(iptr);
			}
		}

		iptrs = retVal;
	}
}
