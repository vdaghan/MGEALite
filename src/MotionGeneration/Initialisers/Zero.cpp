// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Initialisers/Initialisers.h"

namespace MGEA {
	SimulationDataPtrs genesisZero(MotionParameters motionParameters, DEvA::ParameterMap parameters) {
		std::vector<double> zeroVector(motionParameters.simSamples, 0.0);
		SplineControlPoints zeroControlPoints({ {.index = 0, .value = 0.0}, { .index = motionParameters.simSamples-1, .value = 0.0} });

		auto simDataPtr = std::make_shared<SimulationData>();
		simDataPtr->torqueSplines = ClampedSplineMap{};
		for (auto & jointName : motionParameters.jointNames) {
			simDataPtr->torque.emplace(std::make_pair(jointName, zeroVector));
			simDataPtr->torqueSplines->emplace(std::make_pair(jointName, ClampedSpline{ .controlPoints = zeroControlPoints }));
		}
		return {simDataPtr};
	}
}
