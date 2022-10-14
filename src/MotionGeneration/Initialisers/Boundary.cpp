#include "MotionGeneration/Initialisers/Initialisers.h"

namespace MGEA {
	SimulationDataPtrs genesisBoundary(InitialiserParams initialiserParams) {
		SimulationDataPtrs simDataPtrs;

		auto generateBoundaryVector = [&](std::size_t timeIndex, double value) -> std::vector<double> {
			std::vector<double> retVal(initialiserParams.motionParameters.simSamples, 0.0);
			retVal[timeIndex] = value;
			return retVal;
		};

		std::vector<double> const zeroVector(initialiserParams.motionParameters.simSamples, 0.0);
		std::size_t numJoints(initialiserParams.motionParameters.jointNames.size());
		for (std::size_t jointIndex(0); jointIndex != numJoints; ++jointIndex) {
			std::string const& jointName = initialiserParams.motionParameters.jointNames[jointIndex];
			for (std::size_t timeIndex(0); timeIndex != initialiserParams.motionParameters.simSamples; timeIndex += 128) {
				auto& jointLimitsPair = initialiserParams.motionParameters.jointLimits.at(jointName);
				std::array<double, 2> jointLimitsArray{ jointLimitsPair.first, jointLimitsPair.second };
				for (auto& jointLimit : jointLimitsArray) {
					auto simDataPtr = std::make_shared<SimulationData>();
					for (auto& jN : initialiserParams.motionParameters.jointNames) {
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