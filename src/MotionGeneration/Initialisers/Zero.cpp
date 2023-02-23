// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Initialisers/Initialisers.h"

namespace MGEA {
	SimulationDataPtrs genesisZero(MotionParameters motionParameters, DEvA::ParameterMap parameters) {
		std::vector<double> zeroVector(motionParameters.simSamples, 0.0);
		auto simDataPtr = std::make_shared<SimulationData>();
		for (auto & jointName : motionParameters.jointNames) {
			simDataPtr->torque.emplace(std::make_pair(jointName, zeroVector));
		}
		return {simDataPtr};
	}
}
