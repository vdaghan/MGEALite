// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include <algorithm>

namespace MGEA {
	IPtrs cullPartiallyDominated(DEvA::ParameterMap parameters, IPtrs iptrs) {
		std::vector<std::string> metrics(parameters.at("metrics").get<std::vector<std::string>>());
		if (iptrs.empty() or metrics.empty()) {
			return {};
		}

		std::list<Spec::IndividualPtr> retVal{};
		for (auto & metricName : metrics) {
			auto const & minElementIt(std::min_element(iptrs.begin(), iptrs.end(), [&](auto const & lhs, auto const & rhs) -> bool {
				return (lhs->metricMap.at(metricName)).isBetterThan(rhs->metricMap.at(metricName));
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

		return retVal;
	}
}
