// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include "Logging/SpdlogCommon.h"

namespace MGEA {
	void paretoFront(std::vector<std::string> metrics, Spec::IndividualPtrs & iptrs) {
		std::size_t prevCount(iptrs.size());
		auto isDominatedLambda = [&](auto const & iptr) {
			for (auto& other : iptrs) {
				bool isDominatedByOther(true);
				for (auto& metricName : metrics) {
					auto & metricValue(iptr->metricMap.at(metricName));
					auto & otherMetricValue(other->metricMap.at(metricName));
					if (not (otherMetricValue < metricValue)) {
						isDominatedByOther = false;
						break;
					}
				}
				if (isDominatedByOther) {
					return true;
				}
			}
			return false;
		};
		{
			Spec::IndividualPtrs nondominatedIndividuals;
			auto it = std::remove_copy_if(iptrs.begin(), iptrs.end(), std::back_inserter(nondominatedIndividuals), isDominatedLambda);
			iptrs = nondominatedIndividuals;
		}

		iptrs.sort([&](auto& lhs, auto& rhs) {
			auto lhsFitness(lhs->metricMap.at("fitness"));
			auto rhsFitness(rhs->metricMap.at("fitness"));
			return lhsFitness < rhsFitness;
		});

		std::size_t curCount(iptrs.size());
		spdlog::info("\tparetoFront: {} -> {}", prevCount, curCount);
	}
}
