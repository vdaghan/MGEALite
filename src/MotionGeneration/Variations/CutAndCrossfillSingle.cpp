// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

namespace MGEA {
	SimulationDataPtrs cutAndCrossfillSingle(VariationParams variationParameters, Spec::IndividualPtrs iptrs) {
		auto parent1Ptr = iptrs.front();
		auto parent2Ptr = iptrs.back();
		auto const & parent1 = *parent1Ptr->genotype;
		auto const & parent2 = *parent2Ptr->genotype;

		SimulationDataPtr child1DataPtr = std::make_shared<SimulationData>();
		SimulationDataPtr child2DataPtr = std::make_shared<SimulationData>();

		child1DataPtr->time = parent1.time;
		child1DataPtr->params = parent1.params;
		child2DataPtr->time = parent2.time;
		child2DataPtr->params = parent2.params;

		std::size_t const simLength = variationParameters.motionParameters.simSamples;
		std::size_t const numJoints = variationParameters.motionParameters.jointNames.size();
		std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
		std::size_t const randTimeIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, simLength - 1);

		std::size_t i(0);
		std::vector<double> const tmpTorque(simLength);
		for (auto& joints : parent1.torque) {
			auto const& jointName = joints.first;
			auto const& p1JointData = parent1.torque.at(jointName);
			auto const& p2JointData = parent2.torque.at(jointName);
			if (randJointIndex != i) {
				child1DataPtr->torque.emplace(jointName, p1JointData);
				child2DataPtr->torque.emplace(jointName, p2JointData);
				++i;
				continue;
			}
			child1DataPtr->torque.emplace(jointName, tmpTorque);
			child2DataPtr->torque.emplace(jointName, tmpTorque);
			auto& c1JointData = child1DataPtr->torque[jointName];
			auto& c2JointData = child2DataPtr->torque[jointName];
			for (std::size_t ind(0); ind != randTimeIndex; ++ind) {
				c1JointData[ind] = p1JointData[ind];
				c2JointData[ind] = p2JointData[ind];
			}
			for (std::size_t ind(randTimeIndex); ind != simLength; ++ind) {
				c1JointData[ind] = p2JointData[ind];
				c2JointData[ind] = p1JointData[ind];
			}
			++i;
		}

		return { child1DataPtr, child2DataPtr };
	}
}
