// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

namespace MGEA {
	SimulationDataPtrs uniformCrossoverSingle(MotionParameters motionParameters, DEvA::ParameterMap parameters, Spec::IndividualPtrs iptrs) {
		auto parent1Ptr = iptrs.front();
		auto parent2Ptr = iptrs.back();
		auto const & parent1 = *parent1Ptr->genotype;
		auto const & parent2 = *parent2Ptr->genotype;

		SimulationDataPtr child1DataPtr = std::make_shared<SimulationData>();
		SimulationDataPtr child2DataPtr = std::make_shared<SimulationData>();

		child1DataPtr->time = parent1.time;
		child1DataPtr->params = parent1.params;
		child1DataPtr->torque = parent1.torque;
		child2DataPtr->time = parent2.time;
		child2DataPtr->params = parent2.params;
		child2DataPtr->torque = parent2.torque;

		auto choiceLambda = []() {
			return DEvA::RandomNumberGenerator::get()->getIntBetween<int>(0, 1);
		};
		auto numJoints = motionParameters.jointNames.size();
		std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);

		std::size_t jointIndex(0);
		for (auto& joints : parent1.torque) {
			if (randJointIndex != jointIndex) {
				++jointIndex;
				continue;
			}
			auto const& jointName = joints.first;
			auto& c1JointData = child1DataPtr->torque.at(jointName);
			auto& c2JointData = child2DataPtr->torque.at(jointName);
			auto const& p1JointData = parent1.torque.at(jointName);
			auto const& p2JointData = parent2.torque.at(jointName);
			for (std::size_t ind(0); ind != motionParameters.simSamples; ++ind) {
				int const switchParent = choiceLambda();
				if (1 == switchParent) {
					c1JointData.at(ind) = p2JointData.at(ind);
					c2JointData.at(ind) = p1JointData.at(ind);
				}
			}
			break;
		}

		return { child1DataPtr, child2DataPtr };
	}
}
