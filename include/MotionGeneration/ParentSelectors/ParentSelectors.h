#pragma once

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
	static Spec::IndividualPtrs metricProportionalN(std::string metricName, Spec::IndividualPtrs domain) {
		auto bestWorstIteratorPair(std::minmax_element(domain.begin(), domain.end(), [&](auto const & lhs, auto const & rhs) {
			auto const & lhsMetric = lhs->metricMap.at(metricName);
			auto const & rhsMetric = rhs->metricMap.at(metricName);
			return lhsMetric < rhsMetric;
		}));
		auto & worstMetric = (*bestWorstIteratorPair.second)->metricMap.at(metricName);
		auto & bestMetric = (*bestWorstIteratorPair.first)->metricMap.at(metricName);
		std::vector<Spec::IndividualPtr> vec(domain.begin(), domain.end());
		std::vector<double> weights;
		weights.reserve(vec.size());
		auto metricMappingLambda = [&](Spec::IndividualPtr iptr) -> double {
			double metricValue(iptr->metricMap.at(metricName).as<double>());
			double worstMetricValue(worstMetric.as<double>());
			double bestMetricValue(bestMetric.as<double>());
			// t * bestMetric + (t-1) * worstMetric = f
			// t = (f - worstMetric) / (bestMetric + worstMetric)
			double retVal(1.0);
			bool bothZero(0.0 == worstMetricValue and 0.0 == bestMetricValue);
			bool bestWorstEqual(worstMetricValue == bestMetricValue);
			if (!bestWorstEqual and !bothZero) {
				if (bestMetricValue >= worstMetricValue) {
					retVal = (metricValue - worstMetricValue) / std::abs(bestMetricValue + worstMetricValue);
				} else {
					retVal = (metricValue - bestMetricValue) / std::abs(bestMetricValue + worstMetricValue);
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
