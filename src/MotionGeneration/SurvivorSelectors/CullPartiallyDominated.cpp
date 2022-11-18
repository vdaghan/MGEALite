#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include <algorithm>

namespace MGEA {
	void cullPartiallyDominated(std::vector<std::string> metrics, Spec::MetricComparisonMap compMap, Spec::IndividualPtrs& iptrs) {
		std::vector<Spec::IndividualPtr> iPtrVector(iptrs.begin(), iptrs.end());

		auto isPartiallyDominatedLambda = [&](auto & iptr) {
			for (auto & otherptr : iptrs) {
				if (otherptr->id == iptr->id) [[unlikely]] {
					continue;
				}
				std::size_t numDominatingMetrics(0);
				for (auto & metric : metrics) {
					auto const& comp(compMap.at(metric));
					bool otherIsBetter = comp(otherptr->metrics.at(metric), iptr->metrics.at(metric));
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
		for (auto & iptr : iPtrVector) {
			if (!isPartiallyDominatedLambda(iptr)) {
				retVal.push_back(iptr);
			}
		}

		iptrs = retVal;
	}
}
