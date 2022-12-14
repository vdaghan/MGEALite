// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace MGEA {
	SimulationDataPtrs halfSineSingle(VariationParams variationParameters, Spec::IndividualPtrs iptrs) {
		auto parentPtr = iptrs.front();
		auto const & parent = *parentPtr->genotype;

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
		std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
		std::string const & randJointName = variationParameters.motionParameters.jointNames.at(randJointIndex);
		std::pair<double, double> const & jointLimits = variationParameters.motionParameters.jointLimits.at(randJointName);
		double randJointHeight(DEvA::RandomNumberGenerator::get()->getRealBetween<double>(jointLimits.first, jointLimits.second));
		std::size_t const randSignalWidth = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(1, simLength - 1);
		std::size_t const randSignalStartIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, simLength - randSignalWidth - 1);

		std::vector<double> randSignal(halfSineLambda(randSignalWidth, randJointHeight));

		auto & randJointData(childDataPtr->torque.at(randJointName));
		for (std::size_t i(0); i != randSignalWidth; ++i) {
			std::size_t datumIndex(randSignalStartIndex + i);
			randJointData.at(datumIndex) += randSignal.at(i);
			randJointData.at(datumIndex) = std::clamp(randJointData.at(datumIndex), jointLimits.first, jointLimits.second);
		}

		return {childDataPtr};
	}
}
