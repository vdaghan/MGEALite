// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

namespace MGEA {
	SimulationDataPtrs crossoverSingle(MotionParameters motionParameters, DEvA::ParameterMap parameters, Spec::IndividualPtrs iptrs) {
		auto & parent1Ptr = iptrs.front();
		auto & parent2Ptr = iptrs.back();
		auto const & parent1Genotype = *parent1Ptr->genotype;
		auto const & parent2Genotype = *parent2Ptr->genotype;

		SimulationDataPtr child1DataPtr = std::make_shared<SimulationData>();
		SimulationDataPtr child2DataPtr = std::make_shared<SimulationData>();

		child1DataPtr->time = parent1Genotype.time;
		child1DataPtr->params = parent1Genotype.params;
		child2DataPtr->time = parent2Genotype.time;
		child2DataPtr->params = parent2Genotype.params;

		std::size_t const simLength = motionParameters.simSamples;
		std::size_t const numJoints = motionParameters.jointNames.size();
		std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
		double const weight = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(0.0, 1.0);

		std::size_t i(0);
		std::vector<double> const tmpTorque(simLength);
		for (auto & joints : parent1Genotype.torque) {
			auto const & jointName = joints.first;
			auto const & p1JointData = parent1Genotype.torque.at(jointName);
			auto const & p2JointData = parent2Genotype.torque.at(jointName);
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
			for (std::size_t ind(0); ind != simLength; ++ind) {
				c1JointData[ind] = weight * p1JointData[ind] + (1.0 - weight) * p2JointData[ind];
				c2JointData[ind] = (1.0 - weight) * p1JointData[ind] + weight * p2JointData[ind];
			}
			++i;
		}

		return { child1DataPtr, child2DataPtr };
	}
}
