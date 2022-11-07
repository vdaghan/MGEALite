#pragma once

#include "Database.h"
#include "MotionGeneration/MotionParameters.h"
#include "MotionGeneration/SharedSynchronisationHelpers.h"
#include "MotionGeneration/Specification.h"

#include <algorithm>
#include <atomic>
#include <utility>
#include <vector>

namespace MGEA {
	template <std::size_t N>
	static Spec::IndividualPtrs metricProportionalN(std::string metricName, Spec::MetricComparisonMap const & compMap, Spec::IndividualPtrs domain) {
		auto bestWorstIteratorPair(std::minmax_element(domain.begin(), domain.end(), [&](auto const & lhs, auto const & rhs) {
			auto const & lhsFitness = std::get<double>(lhs->metrics.at(metricName));
			auto const & rhsFitness = std::get<double>(rhs->metrics.at(metricName));
			return compMap.at(metricName)(lhsFitness, rhsFitness);
		}));
		double worstFitness = std::get<double>((*bestWorstIteratorPair.second)->metrics.at(metricName));
		double bestFitness = std::get<double>((*bestWorstIteratorPair.first)->metrics.at(metricName));
		std::vector<Spec::IndividualPtr> vec(domain.begin(), domain.end());
		std::vector<double> weights;
		weights.reserve(vec.size());
		auto fitnessMappingLambda = [&](Spec::IndividualPtr iptr) -> double {
			double metric(std::get<double>(iptr->metrics.at(metricName)));
			// t * bestFitness + (t-1) * worstFitness = f
			// t = (f - worstFitness) / (bestFitness + worstFitness)
			double retVal(1.0);
			bool bothZero(0.0 == worstFitness and 0.0 == bestFitness);
			bool bestWorstEqual(worstFitness == bestFitness);
			if (!bestWorstEqual and !bothZero) {
				retVal = (metric - worstFitness) / (bestFitness + worstFitness);
			}
			return retVal;
		};
		std::transform(vec.begin(), vec.end(), std::back_inserter(weights), fitnessMappingLambda);
		std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());

		static std::random_device randomDevice;
		static std::mt19937_64 generator(randomDevice());

		Spec::IndividualPtrs retVal;
		for (std::size_t i(0); i != N; ) {
			std::size_t index = dist(generator);
			auto& v(vec.at(index));
			if (retVal.end() != std::find(retVal.begin(), retVal.end(), v)) {
				continue;
			}
			retVal.push_back(v);
			++i;
		}
		return retVal;
	};

	//template <std::size_t N>
	//static Spec::IndividualPtrs gainProportionalN(Spec::FFitnessComparison isBetter, Spec::IndividualPtrs domain) {
	//	if (domain.empty() or domain.front()->parents.empty() or domain.front()->parents.front()->parents.empty()) {
	//		return {};
	//	}
	//	auto gainLambda = [](auto const & iptr) {
	//		double maximumFitnessDiff(0.0);
	//		for (auto const & parent : iptr->parents) {
	//			auto const & parentFitness(std::get<double>(parent->metrics.at("fitness")));
	//			for (auto const & grandparent : parent->parents) {
	//				auto const & grandparentFitness(std::get<double>(grandparent->metrics.at("fitness")));
	//				double fitnessDiff(parentFitness - grandparentFitness);
	//				maximumFitnessDiff = std::max(maximumFitnessDiff, fitnessDiff);
	//			}
	//		}
	//		return maximumFitnessDiff;
	//	};

	//	auto bestWorstIteratorPair(std::minmax_element(domain.begin(), domain.end(), [&](auto const& lhs, auto const& rhs) {
	//		return isBetter(gainLambda(lhs), gainLambda(rhs));
	//	}));
	//	double worstDiff = gainLambda(*bestWorstIteratorPair.second);
	//	double bestDiff = gainLambda(*bestWorstIteratorPair.first);
	//	std::vector<Spec::IndividualPtr> vec(domain.begin(), domain.end());
	//	std::vector<double> weights;
	//	weights.reserve(vec.size());
	//	auto gainMappingLambda = [&](Spec::IndividualPtr iptr) -> double {
	//		double diff(gainLambda(iptr));
	//		// t * bestFitness + (t-1) * worstFitness = f
	//		// t = (f - worstFitness) / (bestFitness + worstFitness)
	//		double retVal(1.0);
	//		bool bothZero(0.0 == worstDiff and 0.0 == bestDiff);
	//		bool bestWorstEqual(worstDiff == bestDiff);
	//		if (!bestWorstEqual and !bothZero) {
	//			retVal = (diff - worstDiff) / (bestDiff + worstDiff);
	//		}
	//		return retVal;
	//	};
	//	std::transform(vec.begin(), vec.end(), std::back_inserter(weights), gainMappingLambda);
	//	std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());

	//	static std::random_device randomDevice;
	//	static std::mt19937_64 generator(randomDevice());

	//	Spec::IndividualPtrs retVal;
	//	//while (retVal.size() != N) {
	//	//	std::size_t index = dist(generator);
	//	//	retVal.push_back(vec.at(index));

	//	//}
	//	for (std::size_t i(0); i != N; ) {
	//		std::size_t index = dist(generator);
	//		auto& v(vec.at(index));
	//		if (retVal.end() != std::find(retVal.begin(), retVal.end(), v)) {
	//			continue;
	//		}
	//		retVal.push_back(v);
	//		++i;
	//	}
	//	return retVal;
	//};
}
