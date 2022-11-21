#pragma once

#include "Logging/SpdlogCommon.h"
#include "MotionGeneration/Specification.h"

#include <algorithm>
#include <list>
#include <string>
#include <vector>

namespace MGEA {
	void cullEquals(Spec::IndividualPtrs & iptrs);
	void cullPartiallyDominated(std::vector<std::string> metrics, Spec::IndividualPtrs & iptrs);
	template <typename MetricType>
	void onlyPositivesIfThereIsAny(std::string metric, Spec::IndividualPtrs & iptrs) {
		std::size_t prevCount(iptrs.size());
		bool hasNonnegative = std::any_of(iptrs.begin(), iptrs.end(), [&](auto const & iptr) {
			auto const & metricValue(iptr->metricMap.at(metric));
			return metricValue.as<MetricType>() >= MetricType{};
		});
		if (hasNonnegative) {
			auto it = std::remove_if(iptrs.begin(), iptrs.end(), [&](auto & iptr) {
				auto const & metricValue(iptr->metricMap.at(metric));
				return metricValue.as<MetricType>() < MetricType{};
			});
			iptrs.erase(it, iptrs.end());
		}

		std::size_t curCount(iptrs.size());
		spdlog::info("\tonlyPositivesIfThereIsAny: {} -> {}", prevCount, curCount);
	}
	void paretoFront(std::vector<std::string> metrics, Spec::IndividualPtrs & iptrs);
	void survivorSelectionOverMetric(std::string metric, Spec::FSurvivorSelection fSelection, Spec::IndividualPtrs & iptrs);
}
