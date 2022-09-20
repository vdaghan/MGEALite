#include "MotionGeneration/Variations/CrossoverSingle.h"
#include "EvolutionaryAlgorithm.h"

SimulationDataPtrs crossoverSingle(MotionParameters const & motionParameters, SimulationDataPtrs sdptrs) {
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
	double const weight = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(0.0, 1.0);

	std::size_t i(0);
	std::vector<double> const tmpTorque(simLength);
	for (auto & joints : parent1.torque) {
		auto const & jointName = joints.first;
		auto const & p1JointData = parent1.torque.at(jointName);
		auto const & p2JointData = parent2.torque.at(jointName);
		if (randJointIndex != i) {
			child1DataPtr->torque.emplace(jointName, p1JointData);
			child2DataPtr->torque.emplace(jointName, p2JointData);
			++i;
			continue;
		}
		child1DataPtr->torque.emplace(jointName, tmpTorque);
		child2DataPtr->torque.emplace(jointName, tmpTorque);
		auto & c1JointData = child1DataPtr->torque[jointName];
		auto & c2JointData = child2DataPtr->torque[jointName];
		for (std::size_t ind(0); ind != simLength; ++ind) {
			c1JointData[ind] = weight * p1JointData[ind] + (1 - weight) * p2JointData[ind];
			c2JointData[ind] = (1 - weight) * p1JointData[ind] + weight * p2JointData[ind];
		}
		++i;
	}

	return {child1DataPtr, child2DataPtr};
}
