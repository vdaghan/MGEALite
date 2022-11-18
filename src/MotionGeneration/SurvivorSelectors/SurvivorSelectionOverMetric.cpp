#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include "Logging/SpdlogCommon.h"

#include <algorithm>

namespace MGEA {
	void survivorSelectionOverMetric(std::string metric, Spec::FSurvivorSelection fSelection, Spec::IndividualPtrs & iptrs) {
		std::size_t prevCount(iptrs.size());
		std::list<Spec::IndividualPtrs> sets{};
		for (auto it(iptrs.begin()); it != iptrs.end(); ++it) {
			auto & iptr(*it);
			auto & imetric(iptr->metrics.at(metric));
			bool alreadyInSet(false);
			for (auto sit(sets.begin()); sit != sets.end(); ++sit) {
				auto & siptr(sit->front());
				auto & smetric(siptr->metrics.at(metric));
				if (imetric == smetric) {
					alreadyInSet = true;
					break;
				}
			}
			if (alreadyInSet) {
				continue;
			}
			Spec::IndividualPtrs set{};
			set.emplace_back(iptr);
			for (auto nit(std::next(it)); nit != iptrs.end(); ++nit) {
				auto & niptr(*nit);
				auto & nmetric(niptr->metrics.at(metric));
				if (nmetric == imetric) {
					set.emplace_back(niptr);
				}
			}
			sets.emplace_back(set);
		}

		spdlog::info("\tsurvivorSelectionOverMetric: number of different sets having the metric {} = {}", metric, sets.size());
		for (auto & set : sets) {
			fSelection(set);
		}

		Spec::IndividualPtrs retVal{};
		for (auto & set : sets) {
			retVal.insert(retVal.end(), set.begin(), set.end());
		}
		iptrs = retVal;

		std::size_t curCount(iptrs.size());
		spdlog::info("\tsurvivorSelectionOverMetric: {} -> {}", prevCount, curCount);
	}
}
