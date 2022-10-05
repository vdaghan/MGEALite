#include "MotionGeneration/Variations/kPointCrossoverSingle.h"
#include "EvolutionaryAlgorithm.h"

SimulationDataPtrs kPointCrossoverSingle(MotionParameters const & motionParameters, SimulationDataPtrs sdptrs) {
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

	std::size_t const k = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(1, simLength);
	std::vector<std::size_t> kPoints(simLength);
	std::iota(kPoints.begin(), kPoints.end(), 0);
	std::shuffle(kPoints.begin(), kPoints.end(), std::mt19937{std::random_device{}()});
	kPoints.resize(k);
	std::sort(kPoints.begin(), kPoints.end());

	std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);

	bool switchParent(false);

	for (std::size_t ind(0); ind != simLength; ++ind) {
		if (!kPoints.empty() and kPoints.at(0) == ind) {
			switchParent = !switchParent;
		}
		if (switchParent) {
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
				c1JointData.at(ind) = p2JointData.at(ind);
				c2JointData.at(ind) = p1JointData.at(ind);
				break;
			}
		}
	}

	return {child1DataPtr, child2DataPtr};
}
