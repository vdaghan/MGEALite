#include "MotionGeneration/Variations/InsertionSingle.h"
#include "EvolutionaryAlgorithm.h"

SimulationDataPtrs insertionSingle(MotionParameters const& motionParameters, SimulationDataPtrs parents) {
	auto const& parent = *parents.front();

	std::size_t const simLength = motionParameters.simSamples;
	std::size_t const numJoints = motionParameters.jointNames.size();
	if (0 == simLength || 0 == numJoints) {
		return {};
	}

	SimulationDataPtr childDataPtr = SimulationDataPtr(new SimulationData());

	childDataPtr->time = parent.time;
	childDataPtr->params = parent.params;
	childDataPtr->torque = parent.torque;

	std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
	std::string const & randJointName = motionParameters.jointNames.at(randJointIndex);
	std::size_t const randTimeIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, simLength - 1);
	std::pair<double, double> const& jointLimits = motionParameters.jointLimits.at(randJointName);
	double const randTorque = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(jointLimits.first, jointLimits.second);

	std::size_t jointIndex(0);
	for (auto& pair : childDataPtr->torque) {
		if (randJointIndex != jointIndex) {
			++jointIndex;
			continue;
		}
		for (std::size_t i(simLength - 1); i != randTimeIndex; --i) {
			pair.second.at(i) = pair.second.at(i - 1);
		}
		pair.second.at(randTimeIndex) = randTorque;
	}

	return { childDataPtr };
}
