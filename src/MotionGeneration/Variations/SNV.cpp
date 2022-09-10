#include "MotionGeneration/Variations/CutAndCrossfillSingle.h"
#include "EvolutionaryAlgorithm.h"

SimulationDataPtrs snv(SimulationDataPtrs parents) {
	auto & parent = *parents.front();

	std::size_t simLength = parent.time.size();
	std::size_t numJoints = parent.torque.size();
	if (0 == simLength || 0 == numJoints) {
		return {};
	}

	SimulationDataPtr childDataPtr = SimulationDataPtr(new SimulationData());

	childDataPtr->time = parent.time;
	childDataPtr->params = parent.params;
	childDataPtr->torque = parent.torque;

	std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
	std::size_t const randTimeIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, simLength - 1);
	std::size_t const randTorque = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(-100, 100);

	std::size_t i(0);
	for (auto & pair : childDataPtr->torque) {
		if (randJointIndex == i) {
			pair.second.at(randTimeIndex) = randTorque;
			break;
		}
		++i;
	}

	return {childDataPtr};
}
