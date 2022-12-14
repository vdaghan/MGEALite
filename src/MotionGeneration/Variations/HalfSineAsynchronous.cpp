// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace MGEA {
	SimulationDataPtrs halfSineAsynchronous(VariationParams variationParameters, Spec::IndividualPtrs iptrs) {
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
		std::size_t signalWidth(0);
		std::size_t startIndex(0);
		std::size_t signalEndIndex(simLength - 1);
		for (auto& jointName : variationParameters.motionParameters.jointNames) {
			auto& jointTorque(childDataPtr->torque.at(jointName));
			auto& jointLimits(variationParameters.motionParameters.jointLimits.at(jointName));
			signalWidth = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(1, simLength - 1);
			std::size_t minStartIndex(startIndex + 1 >= signalWidth ? startIndex - signalWidth + 1 : 0);
			std::size_t maxStartIndex(simLength - signalWidth - 1);
			startIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(minStartIndex, maxStartIndex);
			double signalHeight(DEvA::RandomNumberGenerator::get()->getRealBetween<double>(jointLimits.first, jointLimits.second));
			std::vector<double> randSignal(halfSineLambda(signalWidth, signalHeight));
			for (std::size_t i(0); i != signalWidth; ++i) {
				std::size_t datumIndex(startIndex + i);
				jointTorque.at(datumIndex) += randSignal.at(i);
				jointTorque.at(datumIndex) = std::clamp(jointTorque.at(datumIndex), jointLimits.first, jointLimits.second);
			}
		}

		return { childDataPtr };
	}
}
