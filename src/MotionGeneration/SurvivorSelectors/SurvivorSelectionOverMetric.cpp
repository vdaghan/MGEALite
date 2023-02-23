// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include "Logging/SpdlogCommon.h"

#include <algorithm>

namespace MGEA {
	void survivorSelectionOverMetric(DEvA::ParameterMap parameters, Spec::FSurvivorSelection fSelection, Spec::IndividualPtrs & iptrs) {
		std::string metric(parameters.at("metric").get<std::string>());

		std::size_t prevCount(iptrs.size());
		std::list<Spec::IndividualPtrs> sets{};
		std::mutex setsMutex{};
		std::for_each(std::execution::par, iptrs.begin(), iptrs.end(), [&](auto const & iptr) -> void {
			auto & imetric(iptr->metricMap.at(metric));
			bool unique(std::none_of(iptrs.begin(), iptrs.end(), [&](auto const & optr){
				auto & ometric(optr->metricMap.at(metric));
				return imetric == ometric;
			}));
			if (unique) {
				Spec::IndividualPtrs set{};
				set.emplace_back(iptr);
				std::lock_guard<std::mutex> lock(setsMutex);
				sets.emplace_back(set);
				return;
			}
			std::lock_guard<std::mutex> lock(setsMutex);
			auto setIterator(std::find_if(std::execution::par, sets.begin(), sets.end(), [&](auto const & set){
				auto & optr(set.front());
				auto & ometric(optr->metricMap.at(metric));
				return imetric == ometric;
			}));
			if (sets.end() == setIterator) {
				Spec::IndividualPtrs set{};
				set.emplace_back(iptr);
				sets.emplace_back(set);
			} else {
				setIterator->emplace_back(iptr);
			}
		});

		spdlog::info("\tsurvivorSelectionOverMetric: number of different values of metric {} in sets = {}", metric, sets.size());
		std::size_t maxSetSizeBeforeSelection(0);
		for (auto & set : sets) {
			maxSetSizeBeforeSelection = std::max(maxSetSizeBeforeSelection, set.size());
		}
		spdlog::info("\tsurvivorSelectionOverMetric: maximum set size before selection = {}", maxSetSizeBeforeSelection);
		for (auto & set : sets) {
			fSelection(set);
		}
		std::size_t maxSetSizeAfterSelection(0);
		for (auto& set : sets) {
			maxSetSizeAfterSelection = std::max(maxSetSizeAfterSelection, set.size());
		}
		spdlog::info("\tsurvivorSelectionOverMetric: maximum set size after selection = {}", maxSetSizeAfterSelection);

		Spec::IndividualPtrs retVal{};
		for (auto & set : sets) {
			retVal.insert(retVal.end(), set.begin(), set.end());
		}
		iptrs = retVal;

		std::size_t curCount(iptrs.size());

		std::stable_sort(iptrs.begin(), iptrs.end(), [&](auto & iptr1, auto & iptr2){
			return iptr1->id < iptr2->id;
		});
		auto last = std::unique(iptrs.begin(), iptrs.end(), [](auto & iptr1, auto & iptr2){
			return iptr1->id == iptr2->id;
		});
		iptrs.erase(last, iptrs.end());

		std::size_t uniqueCount(iptrs.size());
		spdlog::info("\tsurvivorSelectionOverMetric: {} -> {} -> {}", prevCount, curCount, uniqueCount);
	}
}
