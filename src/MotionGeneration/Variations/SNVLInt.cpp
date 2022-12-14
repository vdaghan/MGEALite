// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

namespace MGEA {
	SimulationDataPtrs snvLInt(std::size_t shift, VariationParams variationParameters, Spec::IndividualPtrs iptrs) {
		auto parentPtr = iptrs.front();
		auto const & parent = *parentPtr->genotype;

		SimulationDataPtr childDataPtr = std::make_shared<SimulationData>();

		childDataPtr->time = parent.time;
		childDataPtr->params = parent.params;
		childDataPtr->torque = parent.torque;

		std::size_t const simLength = variationParameters.motionParameters.simSamples;
		std::size_t const numJoints = variationParameters.motionParameters.jointNames.size();
		std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
		std::string const& randJointName = variationParameters.motionParameters.jointNames.at(randJointIndex);
		std::size_t const randControlPointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, (simLength >> shift) - 1);
		std::pair<double, double> const& jointLimits = variationParameters.motionParameters.jointLimits.at(randJointName);
		double const randTorque = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(jointLimits.first, jointLimits.second);

		auto & torqueVector(childDataPtr->torque.at(randJointName));
		std::size_t stride(std::size_t(1) << shift);
		torqueVector.at(randControlPointIndex * stride) = randTorque;
		for (std::size_t i(0); i != simLength; i += stride) {
			double firstValue(torqueVector.at(i));
			double lastValue(0.0);
			if (i + stride != simLength) {
				lastValue = torqueVector.at(i + stride);
			}
			for (std::size_t j(0); j != stride; ++j) {
				double t(double(j) / double(stride));
				double n(firstValue + (lastValue - firstValue) * t);
				torqueVector.at(i + j) = n;
			}
		}

		return { childDataPtr };
	}
}
