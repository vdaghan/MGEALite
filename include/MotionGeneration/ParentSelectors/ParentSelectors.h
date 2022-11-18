#pragma once

#include "Database.h"
#include "MotionGeneration/MotionParameters.h"
#include "MotionGeneration/SharedSynchronisationHelpers.h"
#include "MotionGeneration/Specification.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <utility>
#include <vector>

namespace MGEA {
	template <std::size_t N>
	static Spec::IndividualPtrs metricProportionalN(std::string metricName, Spec::MetricComparisonMap const & compMap, Spec::IndividualPtrs domain) {
		auto & comp(compMap.at(metricName));
		auto bestWorstIteratorPair(std::minmax_element(domain.begin(), domain.end(), [&](auto const & lhs, auto const & rhs) {
			auto const & lhsMetric = std::get<double>(lhs->metrics.at(metricName));
			auto const & rhsMetric = std::get<double>(rhs->metrics.at(metricName));
			return comp(lhsMetric, rhsMetric);
		}));
		double worstMetric = std::get<double>((*bestWorstIteratorPair.second)->metrics.at(metricName));
		double bestMetric = std::get<double>((*bestWorstIteratorPair.first)->metrics.at(metricName));
		std::vector<Spec::IndividualPtr> vec(domain.begin(), domain.end());
		std::vector<double> weights;
		weights.reserve(vec.size());
		auto metricMappingLambda = [&](Spec::IndividualPtr iptr) -> double {
			double metric(std::get<double>(iptr->metrics.at(metricName)));
			// t * bestMetric + (t-1) * worstMetric = f
			// t = (f - worstMetric) / (bestMetric + worstMetric)
			double retVal(1.0);
			bool bothZero(0.0 == worstMetric and 0.0 == bestMetric);
			bool bestWorstEqual(worstMetric == bestMetric);
			if (!bestWorstEqual and !bothZero) {
				if (bestMetric >= worstMetric) {
					retVal = (metric - worstMetric) / std::abs(bestMetric + worstMetric);
				} else {
					retVal = (metric - bestMetric) / std::abs(bestMetric + worstMetric);
				}
			}
			return retVal;
		};
		std::transform(vec.begin(), vec.end(), std::back_inserter(weights), metricMappingLambda);
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
}
