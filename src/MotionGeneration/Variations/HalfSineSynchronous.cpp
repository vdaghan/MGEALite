// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace MGEA {
	SimulationDataPtrs halfSineSynchronous(VariationParams variationParameters, SimulationDataPtrs parents) {
		auto const & parent = *parents.front();

		SimulationDataPtr childDataPtr = std::make_shared<SimulationData>();

		childDataPtr->time = parent.time;
		childDataPtr->params = parent.params;
		childDataPtr->torque = parent.torque;

		auto halfSineLambda = [&](std::size_t signalWidth, double signalHeight) {
			std::vector<double> randSignal(signalWidth, 0.0);
			for (std::size_t i(0); i != signalWidth; ++i) {
				double x(static_cast<double>(i));
				double n(static_cast<double>(signalWidth));
				randSignal.at(i) = signalHeight * std::sin(std::numbers::pi * x / n);
			}
			return randSignal;
		};

		std::size_t const simLength = variationParameters.motionParameters.simSamples;
		std::size_t const numJoints = variationParameters.motionParameters.jointNames.size();
		std::size_t const randSignalWidth = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(1, simLength - 1);
		std::size_t const randSignalStartIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, simLength - randSignalWidth - 1);

		for (auto & jointName : variationParameters.motionParameters.jointNames) {
			auto & jointTorque(childDataPtr->torque.at(jointName));
			auto & jointLimits(variationParameters.motionParameters.jointLimits.at(jointName));
			double randJointHeight(DEvA::RandomNumberGenerator::get()->getRealBetween<double>(jointLimits.first, jointLimits.second));

			std::vector<double> randSignal(halfSineLambda(randSignalWidth, randJointHeight));
			for (std::size_t i(0); i != randSignalWidth; ++i) {
				std::size_t datumIndex(randSignalStartIndex + i);
				jointTorque.at(datumIndex) += randSignal.at(i);
				jointTorque.at(datumIndex) = std::clamp(jointTorque.at(datumIndex), jointLimits.first, jointLimits.second);
			}
		}

		return {childDataPtr};
	}
}
