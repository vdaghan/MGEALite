#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include <algorithm>

namespace MGEA {
	void onlyPositivesIfThereIsAny(std::string metric, Spec::IndividualPtrs& iptrs) {
		bool hasNonnegative = std::any_of(iptrs.begin(), iptrs.end(), [&](auto& iptr) {
			return std::get<double>(iptr->metrics.at(metric)) >= 0;
		});
		if (hasNonnegative) {
			auto it = std::remove_if(iptrs.begin(), iptrs.end(), [&](auto& iptr) {
				return std::get<double>(iptr->metrics.at(metric)) < 0;
			});
			iptrs.erase(it, iptrs.end());
		}
	}
}
