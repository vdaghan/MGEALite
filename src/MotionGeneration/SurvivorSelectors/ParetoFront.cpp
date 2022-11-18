#include "MotionGeneration/SurvivorSelectors/SurvivorSelectors.h"

#include "Logging/SpdlogCommon.h"

namespace MGEA {
	void paretoFront(std::vector<std::string> metrics, Spec::IndividualPtrs & iptrs) {
		std::size_t prevCount(iptrs.size());
		auto isDominatedLambda = [&](auto const & iptr) {
			double fitness = std::get<double>(iptr->metrics.at("fitness"));
			double balance = std::get<double>(iptr->metrics.at("balance"));
			for (auto& other : iptrs) {
				double otherFitness = std::get<double>(other->metrics.at("fitness"));
				double otherBalance = std::get<double>(other->metrics.at("balance"));
				if (otherFitness > fitness and otherBalance < balance) {
					return true;
				}
			}
			return false;
		};
		{
			Spec::IndividualPtrs nondominatedIndividuals;
			auto it = std::remove_copy_if(iptrs.begin(), iptrs.end(), std::back_inserter(nondominatedIndividuals), isDominatedLambda);
			iptrs = nondominatedIndividuals;
		}

		iptrs.sort([&](auto& lhs, auto& rhs) {
			auto lhsFitness(std::get<double>(lhs->metrics.at("fitness")));
			auto rhsFitness(std::get<double>(rhs->metrics.at("fitness")));
			return lhsFitness > rhsFitness;
		});

		std::size_t curCount(iptrs.size());
		spdlog::info("\tparetoFront: {} -> {}", prevCount, curCount);
	}
}
