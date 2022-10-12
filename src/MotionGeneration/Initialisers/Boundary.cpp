#include "MotionGeneration/Initialisers/Initialisers.h"

namespace MGEA {
	Spec::GenotypeProxies genesisBoundary(InitialiserParams initialiserParams) {
		Spec::GenotypeProxies retVal;

		auto generateBoundaryVector = [&](std::size_t timeIndex, double value) -> std::vector<double> {
			std::vector<double> retVal(initialiserParams.motionParameters.simSamples, 0.0);
			retVal[timeIndex] = value;
			return retVal;
		};

		std::vector<double> const zeroVector(initialiserParams.motionParameters.simSamples, 0.0);
		std::size_t numJoints(initialiserParams.motionParameters.jointNames.size());
		std::size_t id(0);
		for (std::size_t jointIndex(0); jointIndex != numJoints; ++jointIndex) {
			std::string const& jointName = initialiserParams.motionParameters.jointNames[jointIndex];
			for (std::size_t timeIndex(0); timeIndex != initialiserParams.motionParameters.simSamples; timeIndex += 128) {
				auto& jointLimitsPair = initialiserParams.motionParameters.jointLimits.at(jointName);
				std::array<double, 2> jointLimitsArray{ jointLimitsPair.first, jointLimitsPair.second };
				for (auto& jointLimit : jointLimitsArray) {
					SimulationInfo simInfo{ .generation = 0, .identifier = id++ };
					auto simDataPtr = initialiserParams.database.createSimulation(simInfo);
					simDataPtr->time = initialiserParams.time;
					simDataPtr->params.emplace("simStart", initialiserParams.motionParameters.simStart);
					simDataPtr->params.emplace("simStop", initialiserParams.motionParameters.simStop());
					simDataPtr->params.emplace("simStep", initialiserParams.motionParameters.simStep);
					simDataPtr->params.emplace("simSamples", static_cast<double>(initialiserParams.motionParameters.simSamples));
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
					retVal.emplace_back(simInfo);
				}
			}
		}
		return retVal;
	}
}
