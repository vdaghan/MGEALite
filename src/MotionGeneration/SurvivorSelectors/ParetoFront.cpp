// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include <algorithm>
#include <ranges>

namespace MGEA {
	bool isDominatedOld(Spec::IndividualPtr const & iptr, Spec::IndividualPtrs const & iptrs, std::vector<std::string> const & metrics) {
		for (auto const & other : iptrs) {
			bool isDominatedByOther(true);
			for (auto const & metricName : metrics) {
				auto const & metricValue(iptr->metricMap.at(metricName));
				auto const & otherMetricValue(other->metricMap.at(metricName));
				if (not (otherMetricValue.isBetterThan(metricValue))) {
					isDominatedByOther = false;
					break;
				}
			}
			if (isDominatedByOther) {
				return true;
			}
		}
		return false;
	}

	bool isDominatedBy(auto const & iptr1, auto const & iptr2, std::vector<std::string> const & metrics) {
		std::size_t dominatedCount = std::ranges::count(metrics, true, [&](auto const & metricName) {
			auto const & metricValue1(iptr1->metricMap.at(metricName));
			auto const & metricValue2(iptr2->metricMap.at(metricName));
			return metricValue2.isBetterThan(metricValue1) or metricValue2.isEquivalentTo(metricValue1);
		});
		return dominatedCount == metrics.size();
	}

	bool isDominatedIn(Spec::IndividualPtr const & iptr, Spec::IndividualPtrs const & iptrs, std::vector<std::string> const & metrics) {
		return std::any_of(std::execution::par, iptrs.begin(), iptrs.end(), [&](auto const & other) -> bool {
			bool sameId(iptr->id == other->id);
			bool dominated(isDominatedBy(iptr, other, metrics));
			return not sameId and dominated;
		});
	}

	IPtrs paretoFront(DEvA::ParameterMap parameters, IPtrs iptrs) {
		std::vector<std::string> metrics(parameters.at("metrics").get<std::vector<std::string>>());
		std::size_t prevCount(iptrs.size());
		auto isDominatedLambda = [&](auto const & iptr) {
			return not isDominatedIn(iptr, iptrs, metrics);
		};

		IPtrs nondominatedIndividuals{};
		std::ranges::copy_if(iptrs.begin(), iptrs.end(), std::back_inserter(nondominatedIndividuals), isDominatedLambda);

		std::size_t curCount(nondominatedIndividuals.size());
		spdlog::info("\tparetoFront: {} -> {}", prevCount, curCount);
		return nondominatedIndividuals;
	}
}
