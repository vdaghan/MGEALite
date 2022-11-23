#pragma once

#include "MotionGeneration/Specification.h"
#include "DEvA/EvolutionaryAlgorithm.h"

#include <map>
#include <string>
#include <vector>

namespace MGEA {
	struct VariationData {
		VariationData() = default;

		std::vector<std::string> lastGenerationVariationNames;
		std::vector<std::size_t> lastGenerationSuccess;
		std::vector<std::size_t> lastGenerationTotal;
		std::vector<double> lastGenerationSuccessRate;
		std::vector<double> lastGenerationTicks;

		std::map<std::string, std::size_t> allGenerationsNameMap;
		std::vector<std::string> allGenerationsVariationNames;
		std::vector<std::size_t> allGenerationsSuccess;
		std::vector<std::size_t> allGenerationsTotal;
		std::vector<double> allGenerationsSuccessRate;
		std::vector<double> allGenerationsTicks;

		std::vector<double> ticks(std::size_t sz) {
			std::vector<double> tickValues;
			for (std::size_t i(0); i != sz; ++i) {
				tickValues.push_back(double(i));
			}
			return tickValues;
		}

		void update(DEvA::EAStatistics<Spec> const & eaStatistics) {
			auto const & vSMap(eaStatistics.variationStatisticsMap);
			lastGenerationVariationNames.clear();
			lastGenerationSuccess.clear();
			lastGenerationTotal.clear();
			lastGenerationSuccessRate.clear();
			for (auto const & vSPair : vSMap) {
				auto const & vSName(vSPair.first);
				auto const & vS(vSPair.second);
				lastGenerationVariationNames.push_back(vSName);
				lastGenerationSuccess.push_back(vS.success);
				lastGenerationTotal.push_back(vS.total);
				lastGenerationSuccessRate.push_back(static_cast<double>(vS.success) / static_cast<double>(vS.total));
				lastGenerationTicks = ticks(lastGenerationVariationNames.size());

				std::size_t nameIndex;
				if (allGenerationsNameMap.contains(vSName)) {
					nameIndex = allGenerationsNameMap.at(vSName);
					allGenerationsSuccess.at(nameIndex) += vS.success;
					allGenerationsTotal.at(nameIndex) += vS.total;
					allGenerationsSuccessRate.at(nameIndex) = static_cast<double>(allGenerationsSuccess.at(nameIndex)) / static_cast<double>(allGenerationsTotal.at(nameIndex));
				} else {
					nameIndex = allGenerationsVariationNames.size();
					allGenerationsVariationNames.push_back(vSName);
					allGenerationsSuccess.push_back(vS.success);
					allGenerationsTotal.push_back(vS.total);
					allGenerationsNameMap.emplace(std::make_pair(vSName, nameIndex));
					allGenerationsSuccessRate.push_back(static_cast<double>(allGenerationsSuccess.at(nameIndex)) / static_cast<double>(allGenerationsTotal.at(nameIndex)));
				}				
				allGenerationsTicks = ticks(allGenerationsVariationNames.size());
			}
		}
	};
};
