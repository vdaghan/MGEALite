// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

#include <cmath>

namespace MGEA {
	SimulationDataPtrs crossoverAll(MotionParameters motionParameters, DEvA::ParameterMap parameters, Spec::IndividualPtrs iptrs) {
		auto & parent1Ptr = iptrs.front();
		auto & parent2Ptr = iptrs.back();
		auto const & parent1Genotype = *parent1Ptr->genotype;
		auto const & parent2Genotype = *parent2Ptr->genotype;

		SimulationDataPtr child1DataPtr = std::make_shared<SimulationData>();
		SimulationDataPtr child2DataPtr = std::make_shared<SimulationData>();

		child1DataPtr->time = parent1Genotype.time;
		child1DataPtr->params = parent1Genotype.params;
		child2DataPtr->time = parent2Genotype.time;
		child2DataPtr->params = parent2Genotype.params;

		double const weight = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(0.0, 1.0);
		std::size_t const simLength = motionParameters.simSamples;

		if (parent1Genotype.torqueSplines and parent2Genotype.torqueSplines) {
			std::map<std::string, std::list<std::size_t>> timePointMap{};
			auto indexOf = [](auto & controlPoint) {
				return controlPoint.index;
			};
			for (auto & [jointName, spline] : parent1Genotype.torqueSplines.value()) {
				if (not timePointMap.contains(jointName)) {
					timePointMap.emplace(jointName, std::list<std::size_t>{});
				}
				auto & controlPoints(spline.controlPoints);
				auto & timePoints(timePointMap.at(jointName));
				std::transform(controlPoints.begin(), controlPoints.end(), std::back_inserter(timePoints), indexOf);
			}
			for (auto & [jointName, spline] : parent2Genotype.torqueSplines.value()) {
				if (not timePointMap.contains(jointName)) {
					timePointMap.emplace(jointName, std::list<std::size_t>{});
				}
				auto & controlPoints(spline.controlPoints);
				auto & timePoints(timePointMap.at(jointName));
				std::transform(controlPoints.begin(), controlPoints.end(), std::back_inserter(timePoints), indexOf);
			}
			for (auto & [jointName, timePoints] : timePointMap) {
				std::stable_sort(timePoints.begin(), timePoints.end());
				auto it = std::unique(timePoints.begin(), timePoints.end());
				timePoints.erase(it, timePoints.end());
			}
			std::map<std::string, std::list<std::pair<std::size_t, std::pair<double, double>>>> torques{};
			for (auto & [jointName, timePoints] : timePointMap) {
				torques.emplace(jointName, std::list<std::pair<std::size_t, std::pair<double, double>>>{});
				auto & torquePairList(torques.at(jointName));
				for (auto & timePoint : timePoints) {
					double const parent1Torque = parent1Genotype.torque.at(jointName).at(timePoint);
					double const parent2Torque = parent2Genotype.torque.at(jointName).at(timePoint);
					torquePairList.emplace_back(timePoint, std::make_pair(parent1Torque, parent2Torque));
				}
			}
			child1DataPtr->torqueSplines = MGEA::ClampedSplineMap{};
			child2DataPtr->torqueSplines = MGEA::ClampedSplineMap{};
			for (auto & [jointName, torquePairList] : torques) {
				child1DataPtr->torqueSplines->emplace(jointName, MGEA::ClampedSpline{});
				child2DataPtr->torqueSplines->emplace(jointName, MGEA::ClampedSpline{});
				for (auto & torquePair : torquePairList) {
					auto parent1Torque(torquePair.second.first);
					auto parent2Torque(torquePair.second.second);
					//auto child1Torque(weight * parent1Torque + (1 - weight) * parent2Torque);
					//auto child2Torque((1 - weight) * parent1Torque + weight * parent2Torque);
					auto child1Torque(std::lerp(parent2Torque, parent1Torque, weight));
					auto child2Torque(std::lerp(parent1Torque, parent2Torque, weight));
					child1DataPtr->torqueSplines->at(jointName).addControlPoint({ .index = torquePair.first, .value = child1Torque });
					child2DataPtr->torqueSplines->at(jointName).addControlPoint({ .index = torquePair.first, .value = child2Torque });
				}
				child1DataPtr->torque[jointName] = child1DataPtr->torqueSplines->at(jointName).evaluate();
				child2DataPtr->torque[jointName] = child2DataPtr->torqueSplines->at(jointName).evaluate();
			}
		} else {
			std::vector<double> const tmpTorque(simLength);
			for (auto & joints : parent1Genotype.torque) {
				auto const & jointName = joints.first;
				auto const & p1JointData = parent1Genotype.torque.at(jointName);
				auto const & p2JointData = parent2Genotype.torque.at(jointName);
				child1DataPtr->torque.emplace(jointName, tmpTorque);
				child2DataPtr->torque.emplace(jointName, tmpTorque);
				auto & c1JointData = child1DataPtr->torque[jointName];
				auto & c2JointData = child2DataPtr->torque[jointName];
				for (std::size_t ind(0); ind != simLength; ++ind) {
					c1JointData[ind] = weight * p1JointData[ind] + (1 - weight) * p2JointData[ind];
					c2JointData[ind] = (1 - weight) * p1JointData[ind] + weight * p2JointData[ind];
				}
			}
		}

		return { child1DataPtr, child2DataPtr };
	}
}
