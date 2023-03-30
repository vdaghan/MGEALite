// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

namespace MGEA {
	SimulationDataPtrs insertionSingle(MotionParameters motionParameters, DEvA::ParameterMap parameters, Spec::IndividualPtrs iptrs) {
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
		//double const randTorque = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(jointLimits.first, jointLimits.second);

		auto & jointTorque(childDataPtr->torque.at(randJointName));
		if (childDataPtr->torqueSplines) {
			auto & jointSpline(childDataPtr->torqueSplines->at(randJointName));
			auto randTorque(generateCenteredRandomDouble(jointLimits.first, jointTorque.at(randTimeIndex), jointLimits.second));
			jointSpline.addControlPoint({ .index = randTimeIndex, .value = randTorque });
			jointTorque = jointSpline.evaluate();
		} else {
			auto randTorque(generateCenteredRandomDouble(jointLimits.first, jointTorque.at(randTimeIndex), jointLimits.second));
			for (std::size_t i(simLength - 1); i != randTimeIndex; --i) {
				jointTorque.at(i) = jointTorque.at(i - 1);
			}
			jointTorque.at(randTimeIndex) = randTorque;
		}

		return { childDataPtr };
	}
}
