// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include "Logging/SpdlogCommon.h"

#include <algorithm>

namespace MGEA {
	IPtrs cullPartiallyDominated(DEvA::ParameterMap parameters, IPtrs iptrs) {
		std::vector<std::string> metrics(parameters.at("metrics").get<std::vector<std::string>>());
		if (iptrs.empty() or metrics.empty()) {
			return {};
		}

		auto cullLambda = [&metrics](auto iptrs) {
			IPtrs retVal{};
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
		};
		
		if (parameters.contains("clusterMetric")) {
			std::size_t prevCount(iptrs.size());
			std::string clusterMetric(parameters.at("clusterMetric").get<std::string>());
			auto clusters(clusterOverMetric(clusterMetric, iptrs));

			spdlog::info("\cullPartiallyDominated: number of different values of metric {} in clusters = {}", clusterMetric, clusters.size());
			std::size_t maxSetSizeBeforeSelection(0);
			for (auto & cluster : clusters) {
				maxSetSizeBeforeSelection = std::max(maxSetSizeBeforeSelection, cluster.size());
			}
			spdlog::info("\cullPartiallyDominated: maximum cluster size before selection = {}", maxSetSizeBeforeSelection);
			for (auto & cluster : clusters) {
				cluster = cullLambda(cluster);
			}
			std::size_t maxSetSizeAfterSelection(0);
			for (auto & cluster : clusters) {
				maxSetSizeAfterSelection = std::max(maxSetSizeAfterSelection, cluster.size());
			}
			spdlog::info("\cullPartiallyDominated: maximum cluster size after selection = {}", maxSetSizeAfterSelection);

			Spec::IndividualPtrs retVal{};
			for (auto & cluster : clusters) {
				retVal.insert(retVal.end(), cluster.begin(), cluster.end());
			}
			iptrs = retVal;

			std::size_t curCount(iptrs.size());

			std::stable_sort(retVal.begin(), retVal.end(), [&](auto & iptr1, auto & iptr2) { return iptr1->id < iptr2->id; });
			auto last = std::unique(retVal.begin(), retVal.end(), [](auto & iptr1, auto & iptr2) { return iptr1->id == iptr2->id; });
			retVal.erase(last, retVal.end());

			std::size_t uniqueCount(retVal.size());
			spdlog::info("\cullPartiallyDominated: {} -> {} -> {}", prevCount, curCount, uniqueCount);
			return retVal;
		} else {
			return cullLambda(iptrs);
		}

	}
}
