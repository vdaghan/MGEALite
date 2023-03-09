#pragma once

#include "Logging/SpdlogCommon.h"
#include "MotionGeneration/Specification.h"

#include <algorithm>
#include <list>
#include <string>
#include <vector>

namespace MGEA {
	using IPtrs = Spec::IndividualPtrs;
	IPtrs cullEquals(DEvA::ParameterMap, IPtrs);
	IPtrs cullPartiallyDominated(DEvA::ParameterMap, IPtrs);
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
	IPtrs paretoFront(DEvA::ParameterMap, IPtrs);
	void survivorSelectionOverMetric(DEvA::ParameterMap, Spec::BPSurvivorSelection::Function fSelection, Spec::IndividualPtrs & iptrs);
}
