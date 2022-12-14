// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

namespace MGEA {
	SimulationDataPtrs deletionLInt(std::size_t shift, VariationParams variationParameters, Spec::IndividualPtrs iptrs) {
		auto parentPtr = iptrs.front();
		auto const & parent = *parentPtr->genotype;

		SimulationDataPtr childDataPtr = std::make_shared<SimulationData>();

		childDataPtr->time = parent.time;
		childDataPtr->params = parent.params;
		childDataPtr->torque = parent.torque;

		std::size_t const simLength = variationParameters.motionParameters.simSamples;
		std::size_t const numJoints = variationParameters.motionParameters.jointNames.size();
		//std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
		//std::string const& randJointName = variationParameters.motionParameters.jointNames.at(randJointIndex);
		std::size_t const numControlPoints(simLength >> shift);
		std::size_t const randControlPointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numControlPoints - 1);
		//std::pair<double, double> const& jointLimits = variationParameters.motionParameters.jointLimits.at(randJointName);
		//double const randTorque = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(jointLimits.first, jointLimits.second);

		std::size_t stride(std::size_t(1) << shift);

		for (auto const & jointName : variationParameters.motionParameters.jointNames) {
			auto& torqueVector(childDataPtr->torque.at(jointName));

			std::vector<double> controlPoints(numControlPoints);
			{
				std::size_t i(0);
				for (; i < randControlPointIndex; ++i) {
					controlPoints[i] = torqueVector[i * stride];
				}
				for (; (i+1) * stride < simLength; ++i) {
					controlPoints[i] = torqueVector[(i+1) * stride];
				}
				controlPoints[i] = 0.0;
			}
			//for (std::size_t i(0); i * stride < simLength; ++i) {
			//	controlPoints[i] = torqueVector[i * stride];
			//}
			//for (std::size_t i(randControlPointIndex); (i + 1) * stride < simLength; ++i) {
			//	controlPoints[i] = controlPoints[i + 1];
			//}
			// TODO:: Think of the last index

			for (std::size_t i(0); i * stride < simLength; ++i) {
				double firstValue(controlPoints[i]);
				double lastValue(0.0);
				if ((i + 1) * stride < simLength) {
					lastValue = controlPoints[i + 1];
				}
				for (std::size_t j(0); j != stride; ++j) {
					double t(double(j) / double(stride));
					double n(firstValue + (lastValue - firstValue) * t);
					torqueVector.at(i * stride + j) = n;
				}
			}
		}

		return { childDataPtr };
	}
}
