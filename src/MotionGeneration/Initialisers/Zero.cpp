#include "MotionGeneration/Initialisers/Initialisers.h"

namespace MGEA {
	SimulationDataPtrs genesisZero(InitialiserParams initialiserParams) {
		std::vector<double> zeroVector(initialiserParams.motionParameters.simSamples, 0.0);
		auto simDataPtr = std::make_shared<SimulationData>();
		for (auto & jointName : initialiserParams.motionParameters.jointNames) {
			simDataPtr->torque.emplace(std::make_pair(jointName, zeroVector));
		}
		return {simDataPtr};
	}
}
