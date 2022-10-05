#include "MotionGeneration/Variations/UniformCrossoverSingle.h"
#include "EvolutionaryAlgorithm.h"

SimulationDataPtrs uniformCrossoverSingle(MotionParameters const & motionParameters, SimulationDataPtrs sdptrs) {
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
	child1DataPtr->torque = parent1.torque;
	child2DataPtr->time = parent2.time;
	child2DataPtr->params = parent2.params;
	child2DataPtr->torque = parent2.torque;

	auto choiceLambda = []() {
		return DEvA::RandomNumberGenerator::get()->getIntBetween<int>(0, 1);
	};
	std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);

	std::size_t jointIndex(0);
	for (auto & joints : parent1.torque) {
		if (randJointIndex != jointIndex) {
			++jointIndex;
			continue;
		}
		auto const & jointName = joints.first;
		auto & c1JointData = child1DataPtr->torque.at(jointName);
		auto & c2JointData = child2DataPtr->torque.at(jointName);
		auto const & p1JointData = parent1.torque.at(jointName);
		auto const & p2JointData = parent2.torque.at(jointName);
		for (std::size_t ind(0); ind != simLength; ++ind) {
			int const switchParent = choiceLambda();
			if (1 == switchParent) {
				c1JointData.at(ind) = p2JointData.at(ind);
				c2JointData.at(ind) = p1JointData.at(ind);
			}
		}
		break;
	}

	return {child1DataPtr, child2DataPtr};
}
