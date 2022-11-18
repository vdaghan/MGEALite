#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include "Logging/SpdlogCommon.h"

#include <algorithm>

namespace MGEA {
	void onlyPositivesIfThereIsAny(std::string metric, Spec::IndividualPtrs& iptrs) {
		std::size_t prevCount(iptrs.size());
		bool hasNonnegative = std::any_of(iptrs.begin(), iptrs.end(), [&](auto& iptr) {
			return std::get<double>(iptr->metrics.at(metric)) >= 0;
		});
		if (hasNonnegative) {
			auto it = std::remove_if(iptrs.begin(), iptrs.end(), [&](auto& iptr) {
				return std::get<double>(iptr->metrics.at(metric)) < 0;
			});
			iptrs.erase(it, iptrs.end());
		}

		std::size_t curCount(iptrs.size());
		spdlog::info("\tonlyPositivesIfThereIsAny: {} -> {}", prevCount, curCount);
	}
}
