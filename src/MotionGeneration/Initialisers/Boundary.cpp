// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Initialisers/Initialisers.h"

namespace MGEA {
	SimulationDataPtrs genesisBoundary(MotionParameters motionParameters, DEvA::ParameterMap parameters) {
		SimulationDataPtrs simDataPtrs;

		auto generateBoundaryVector = [&](std::size_t timeIndex, double value) -> std::vector<double> {
			std::vector<double> retVal(motionParameters.simSamples, 0.0);
			retVal[timeIndex] = value;
			return retVal;
		};

		std::vector<double> const zeroVector(motionParameters.simSamples, 0.0);
		std::size_t numJoints(motionParameters.jointNames.size());
		for (std::size_t jointIndex(0); jointIndex != numJoints; ++jointIndex) {
			std::string const& jointName = motionParameters.jointNames[jointIndex];
			for (std::size_t timeIndex(0); timeIndex < motionParameters.simSamples; ++timeIndex) {
				auto& jointLimitsPair = motionParameters.jointLimits.at(jointName);
				std::array<double, 2> jointLimitsArray{ jointLimitsPair.first, jointLimitsPair.second };
				for (auto& jointLimit : jointLimitsArray) {
					auto simDataPtr = std::make_shared<SimulationData>();
					for (auto& jN : motionParameters.jointNames) {
						std::vector<double> boundaryVector;
						if (jointName != jN) {
							boundaryVector = zeroVector;
						}
						else {
							boundaryVector = generateBoundaryVector(timeIndex, jointLimit);
						}
						simDataPtr->torque.emplace(std::make_pair(jN, boundaryVector));
					}
					simDataPtrs.emplace_back(simDataPtr);
				}
			}
		}
		return simDataPtrs;
	}
}
