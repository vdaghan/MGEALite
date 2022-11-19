#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include "Logging/SpdlogCommon.h"

#include <algorithm>

namespace MGEA {
	void cullEquals(Spec::IndividualPtrs & iptrs) {
		std::size_t prevCount(iptrs.size());
		Spec::IndividualPtrs retVal{};
		for (auto it(iptrs.begin()); it != iptrs.end(); ++it) {
			bool thereIsAnEqual(false);
			for (auto otherit(std::next(it)); otherit != iptrs.end(); ++otherit) {
				auto & iptr(*it);
				auto & otheriptr(*otherit);
				if (iptr->genotype->torque == otheriptr->genotype->torque) {
					thereIsAnEqual = true;
					break;
				}
			}
			if (!thereIsAnEqual) {
				retVal.push_back(*it);
			}
		}

		iptrs = retVal;

		std::size_t curCount(iptrs.size());
		spdlog::info("\tcullEquals: {} -> {}", prevCount, curCount);
	}
}
