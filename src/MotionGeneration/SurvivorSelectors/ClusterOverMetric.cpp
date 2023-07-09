// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include <algorithm>

namespace MGEA {
	std::list<IPtrs> clusterOverMetric(std::string metric, IPtrs iptrs) {
		std::size_t prevCount(iptrs.size());
		std::list<Spec::IndividualPtrs> clusters{};
		std::mutex clustersMutex{};
		std::for_each(std::execution::par, iptrs.begin(), iptrs.end(), [&](auto const & iptr) -> void {
			auto & imetric(iptr->metricMap.at(metric));
			bool unique(std::none_of(iptrs.begin(), iptrs.end(), [&](auto const & optr) {
				auto & ometric(optr->metricMap.at(metric));
				return imetric.isEquivalentTo(ometric);
				}));
			if (unique) {
				Spec::IndividualPtrs cluster{};
				cluster.emplace_back(iptr);
				std::lock_guard<std::mutex> lock(clustersMutex);
				clusters.emplace_back(cluster);
				return;
			}
			std::lock_guard<std::mutex> lock(clustersMutex);
			auto setIterator(std::find_if(std::execution::par, clusters.begin(), clusters.end(), [&](auto const & set) {
				auto & optr(set.front());
				auto & ometric(optr->metricMap.at(metric));
				return imetric.isEquivalentTo(ometric);
				}));
			if (clusters.end() == setIterator) {
				Spec::IndividualPtrs cluster{};
				cluster.emplace_back(iptr);
				clusters.emplace_back(cluster);
			} else {
				setIterator->emplace_back(iptr);
			}
			});

		return clusters;
	}
}
