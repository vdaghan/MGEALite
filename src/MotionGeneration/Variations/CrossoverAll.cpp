// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"

namespace MGEA {
	SimulationDataPtrs crossoverAll(VariationParams variationParameters, SimulationDataPtrs sdptrs) {
		auto const& parent1 = *sdptrs.front();
		auto const& parent2 = *sdptrs.back();

		SimulationDataPtr child1DataPtr = std::make_shared<SimulationData>();
		SimulationDataPtr child2DataPtr = std::make_shared<SimulationData>();

		child1DataPtr->time = parent1.time;
		child1DataPtr->params = parent1.params;
		child2DataPtr->time = parent2.time;
		child2DataPtr->params = parent2.params;

		double const weight = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(0.0, 1.0);

		std::size_t const simLength = variationParameters.motionParameters.simSamples;
		std::vector<double> const tmpTorque(simLength);
		for (auto& joints : parent1.torque) {
			auto const& jointName = joints.first;
			auto const& p1JointData = parent1.torque.at(jointName);
			auto const& p2JointData = parent2.torque.at(jointName);
			child1DataPtr->torque.emplace(jointName, tmpTorque);
			child2DataPtr->torque.emplace(jointName, tmpTorque);
			auto& c1JointData = child1DataPtr->torque[jointName];
			auto& c2JointData = child2DataPtr->torque[jointName];
			for (std::size_t ind(0); ind != simLength; ++ind) {
				c1JointData[ind] = weight * p1JointData[ind] + (1 - weight) * p2JointData[ind];
				c2JointData[ind] = (1 - weight) * p1JointData[ind] + weight * p2JointData[ind];
			}
		}

		return { child1DataPtr, child2DataPtr };
	}
}
