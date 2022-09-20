#include "MotionGeneration/Variations/CutAndCrossfillAll.h"
#include "EvolutionaryAlgorithm.h"

SimulationDataPtrs cutAndCrossfillAll(MotionParameters const & motionParameters, SimulationDataPtrs sdptrs) {
	auto const & parent1 = *sdptrs.front();
	auto const & parent2 = *sdptrs.back();

	std::size_t const simLength = motionParameters.simSamples;
	std::size_t const numJoints = motionParameters.jointNames.size();
	if (0 == simLength || 0 == numJoints) {
		return {};
	}

	SimulationDataPtr child1DataPtr = SimulationDataPtr(new SimulationData());
	SimulationDataPtr child2DataPtr = SimulationDataPtr(new SimulationData());

	child1DataPtr->time = parent1.time;
	child1DataPtr->params = parent1.params;
	child2DataPtr->time = parent2.time;
	child2DataPtr->params = parent2.params;

	std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
	std::size_t const randTimeIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, simLength - 1);

	std::vector<double> const tmpTorque(simLength);
	for (auto & joints : parent1.torque) {
		auto const & jointName = joints.first;
		auto const & p1JointData = parent1.torque.at(jointName);
		auto const & p2JointData = parent2.torque.at(jointName);
		child1DataPtr->torque.emplace(jointName, tmpTorque);
		child2DataPtr->torque.emplace(jointName, tmpTorque);
		auto & c1JointData = child1DataPtr->torque[jointName];
		auto & c2JointData = child2DataPtr->torque[jointName];
		for (std::size_t ind(0); ind != randTimeIndex; ++ind) {
			c1JointData[ind] = p1JointData[ind];
			c2JointData[ind] = p2JointData[ind];
		}
		for (std::size_t ind(randTimeIndex); ind != simLength; ++ind) {
			c1JointData[ind] = p2JointData[ind];
			c2JointData[ind] = p1JointData[ind];
		}
	}

	return {child1DataPtr, child2DataPtr};
}
