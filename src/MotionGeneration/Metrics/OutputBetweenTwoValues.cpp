// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Metrics/Metrics.h"

#include <any>
#include <cmath>
#include <numbers>
#include <vector>

namespace MGEA {
	std::any outputBetweenTwoValues(DEvA::ParameterMap parameterMap, Spec::IndividualPtr iptr) {
		std::string valueName(parameterMap.at("value").get<std::string>());
		std::string firstBoundaryName(parameterMap.at("firstBoundary").get<std::string>());
		std::string secondBoundaryName(parameterMap.at("secondBoundary").get<std::string>());

		auto & simDataPtr(*iptr->maybePhenotype);

		auto & value = simDataPtr->outputs.at(valueName);
		auto & firstBoundary = simDataPtr->outputs.at(firstBoundaryName);
		auto & secondBoundary = simDataPtr->outputs.at(secondBoundaryName);
		std::vector<double> distanceVector{};

		auto valueIt(value.begin());
		auto firstBoundaryIt(firstBoundary.begin());
		auto secondBoundaryIt(secondBoundary.begin());
		while (valueIt != value.end()) {
			auto v(*valueIt);
			auto fb(*firstBoundaryIt);
			auto sb(*secondBoundaryIt);
			if (v <= std::max(fb, sb) and v >= std::min(fb, sb)) {
				distanceVector.emplace_back(0.0);
			} else {
				auto distance(std::min(std::abs(v - fb), std::abs(v - sb)));
				distanceVector.emplace_back(distance);
			}

			++valueIt;
			++firstBoundaryIt;
			++secondBoundaryIt;
		}

		double distance(std::accumulate(distanceVector.begin(), distanceVector.end(), 0.0));
		double timeStep(iptr->genotype->params.at("simStep"));
		double timeStop(iptr->genotype->params.at("simStop"));
		distance = distance * timeStep / timeStop;
		return distance;
	}
}
