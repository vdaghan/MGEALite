// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include <algorithm>

namespace MGEA {
	IPtrs cullEquals(DEvA::ParameterMap parameters, IPtrs iptrs) {
		std::size_t prevCount(iptrs.size());
		//Spec::IndividualPtrs retVal{};
		//for (auto it(iptrs.begin()); it != iptrs.end(); ++it) {
		//	bool thereIsAnEqual(false);
		//	for (auto otherit(std::next(it)); otherit != iptrs.end(); ++otherit) {
		//		auto & iptr(*it);
		//		auto & otheriptr(*otherit);
		//		if (iptr->id == otheriptr->id) {
		//			continue;
		//		}
		//		if (iptr->genotype->torque == otheriptr->genotype->torque) {
		//			thereIsAnEqual = true;
		//			break;
		//		}
		//	}
		//	if (!thereIsAnEqual) {
		//		retVal.push_back(*it);
		//	}
		//}

		IPtrs retVal(iptrs);
		auto last = std::unique(std::execution::par, retVal.begin(), retVal.end(), [](auto & lhs, auto & rhs) {
			return lhs->id == rhs->id or lhs->genotype->torque == rhs->genotype->torque;
		});
		retVal.erase(last, retVal.end());

		std::size_t curCount(retVal.size());
		spdlog::info("\tcullEquals: {} -> {}", prevCount, curCount);
		return retVal;
	}
}
