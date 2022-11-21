#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include <algorithm>

namespace MGEA {
	void cullPartiallyDominated(std::vector<std::string> metrics, Spec::IndividualPtrs & iptrs) {
		if (iptrs.empty() or metrics.empty()) {
			return;
		}

		std::list<Spec::IndividualPtr> retVal{};
		for (auto & metricName : metrics) {
			auto const & minElementIt(std::min_element(iptrs.begin(), iptrs.end(), [&](auto const & lhs, auto const & rhs) -> bool {
				return (lhs->metricMap.at(metricName)) < (rhs->metricMap.at(metricName));
			}));
			if (minElementIt != iptrs.end()) {
				auto const & minElement(*minElementIt);
				bool alreadyExists(false);
				for (auto const & iptr : retVal) {
					if (iptr->id == minElement->id) {
						alreadyExists = true;
					}
				}
				if (!alreadyExists) {
					retVal.push_back(minElement);
				}
			}
		}

		iptrs = retVal;
	}
}
