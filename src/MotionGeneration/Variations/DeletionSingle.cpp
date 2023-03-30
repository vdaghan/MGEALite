// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

namespace MGEA {
	SimulationDataPtrs deletionSingle(MotionParameters motionParameters, DEvA::ParameterMap parameters, Spec::IndividualPtrs iptrs) {
		auto parentPtr = iptrs.front();
		auto const & parent = *parentPtr->genotype;

		SimulationDataPtr childDataPtr = std::make_shared<SimulationData>();

		childDataPtr->time = parent.time;
		childDataPtr->params = parent.params;
		childDataPtr->torque = parent.torque;
		childDataPtr->torqueSplines = parent.torqueSplines;

		std::size_t const simLength = motionParameters.simSamples;
		std::size_t const numJoints = motionParameters.jointNames.size();
		std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
		std::string const& randJointName = motionParameters.jointNames.at(randJointIndex);
		std::size_t const randTimeIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, simLength - 1);
		std::pair<double, double> const& jointLimits = motionParameters.jointLimits.at(randJointName);
		double const randTorque = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(jointLimits.first, jointLimits.second);

		auto & jointTorque(childDataPtr->torque.at(randJointName));
		if (childDataPtr->torqueSplines) {
			auto & splines(childDataPtr->torqueSplines.value());
			auto & spline(splines.at(randJointName));
			spline.removeTimePointAndShiftRest(randTimeIndex);
			jointTorque = spline.evaluate();
			//std::vector<std::pair<std::string, std::size_t>> splinePairs{};
			//for (auto & [jointName, controlPointIndex] : childDataPtr->torqueSplines.value()) {
			//	for (std::size_t i(1); i < splines.at(jointName).controlPoints.size() - 1; ++i) {
			//		splinePairs.push_back(std::make_pair(jointName, i));
			//	}
			//}
			//if (splinePairs.empty()) {
			//	return {};
			//}
			//std::size_t const randomDeletionIndex(DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, splinePairs.size() - 1));
			//auto & splinePair(splinePairs.at(randomDeletionIndex));
			//auto & spline(splines.at(splinePair.first));
			////splines.at(splinePair.first).removeNthControlPointAndShiftRest(splinePair.second);
			//splines.at(splinePair.first).removeTimePointAndShiftRest(splinePair.second);
			//childDataPtr->torque.at(splinePair.first) = spline.evaluate();
		} else {
			for (std::size_t i(randTimeIndex); i < simLength - 1; ++i) {
				jointTorque.at(i) = jointTorque.at(i + 1);
			}
			jointTorque.at(simLength - 1) = 0.0;
		}

		return { childDataPtr };
	}
}
