#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include <algorithm>

namespace MGEA {
	void cullPartiallyDominated(std::vector<std::string> metrics, Spec::MetricComparisonMap compMap, Spec::IndividualPtrs& iptrs) {
		std::vector<Spec::IndividualPtr> iPtrVector(iptrs.begin(), iptrs.end());
		auto metricAccessorLambda = [](Spec::IndividualPtr & iptr, std::string metric) -> double {
			return std::get<double>(iptr->metrics.at(metric));
		};
		auto isPartiallyDominatedLambda = [&](auto & iptr) {
			return std::any_of(iptrs.begin(), iptrs.end(), [&](auto & otherptr) {
				for (auto & metric : metrics) {
					auto const & comp(compMap.at(metric));
					bool otherIsBetter = comp(otherptr->metrics.at(metric), iptr->metrics.at(metric));
					if (otherIsBetter) {
						return true;
					}
				}
				return false;
			});
		};

		std::list<Spec::IndividualPtr> retVal{};
		std::copy_if(iPtrVector.begin(), iPtrVector.end(), std::back_inserter(retVal), [&](auto & iptr) {
			return !isPartiallyDominatedLambda(iptr);
		});

		iptrs = retVal;
	}
}
