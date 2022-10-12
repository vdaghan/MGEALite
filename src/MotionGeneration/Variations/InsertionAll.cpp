#include "MotionGeneration/Variations/Variations.h"

namespace MGEA {
	SimulationDataPtrs insertionAll(VariationParams variationParameters, SimulationDataPtrs parents) {
		auto const& parent = *parents.front();

		SimulationDataPtr childDataPtr = SimulationDataPtr(new SimulationData());

		childDataPtr->time = parent.time;
		childDataPtr->params = parent.params;
		childDataPtr->torque = parent.torque;

		std::size_t const simLength = variationParameters.motionParameters.simSamples;
		std::size_t const numJoints = variationParameters.motionParameters.jointNames.size();
		std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
		std::string const& randJointName = variationParameters.motionParameters.jointNames.at(randJointIndex);
		std::size_t const randTimeIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, simLength - 1);
		std::pair<double, double> const& jointLimits = variationParameters.motionParameters.jointLimits.at(randJointName);
		double const randTorque = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(jointLimits.first, jointLimits.second);

		for (auto& pair : childDataPtr->torque) {
			for (std::size_t i(simLength - 1); i != randTimeIndex; --i) {
				pair.second.at(i) = pair.second.at(i - 1);
			}
			pair.second.at(randTimeIndex) = randTorque;
		}

		return { childDataPtr };
	}
}
