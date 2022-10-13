#include "MotionGeneration/Initialisers/Initialisers.h"

namespace MGEA {
	SimulationDataPtrs genesisRandom(std::size_t numIndividuals, InitialiserParams initialiserParams) {
		SimulationDataPtrs simDataPtrs;

		auto generateRandomVector = [&](std::pair<double, double> limits) -> std::vector<double> {
			std::vector<double> retVal(initialiserParams.motionParameters.simSamples);
			auto vectorGenerator = [&]() {
				return DEvA::RandomNumberGenerator::get()->getRealBetween<double>(limits.first, limits.second);
			};
			std::generate(retVal.begin(), retVal.end(), vectorGenerator);
			return retVal;
		};

		for (size_t n(0); n != numIndividuals; ++n) {
			auto simDataPtr = std::make_shared<SimulationData>();
			for (auto& jointName : initialiserParams.motionParameters.jointNames) {
				auto& jointLimits = initialiserParams.motionParameters.jointLimits.at(jointName);
				auto randomVector = generateRandomVector(jointLimits);
				simDataPtr->torque.emplace(std::make_pair(jointName, randomVector));
			}
			simDataPtrs.emplace_back(simDataPtr);
		}
		return simDataPtrs;
	}
}
