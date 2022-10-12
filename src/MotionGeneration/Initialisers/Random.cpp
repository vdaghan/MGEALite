#include "MotionGeneration/Initialisers/Initialisers.h"

namespace MGEA {
	Spec::GenotypeProxies genesisRandom(std::size_t numIndividuals, InitialiserParams initialiserParams) {
		Spec::GenotypeProxies retVal;

		auto generateRandomVector = [&](std::pair<double, double> limits) -> std::vector<double> {
			std::vector<double> retVal(initialiserParams.motionParameters.simSamples);
			auto vectorGenerator = [&]() {
				return DEvA::RandomNumberGenerator::get()->getRealBetween<double>(limits.first, limits.second);
			};
			std::generate(retVal.begin(), retVal.end(), vectorGenerator);
			return retVal;
		};

		for (size_t n(0); n != numIndividuals; ++n) {
			if (checkStopFlagAndMaybeWait(initialiserParams.pauseFlag, initialiserParams.stopFlag)) {
				return {};
			}
			SimulationInfo simInfo{ .generation = 0, .identifier = n };
			auto simDataPtr = initialiserParams.database.createSimulation(simInfo);
			simDataPtr->time = initialiserParams.time;;
			simDataPtr->params.emplace("simStart", initialiserParams.motionParameters.simStart);
			simDataPtr->params.emplace("simStop", initialiserParams.motionParameters.simStop());
			simDataPtr->params.emplace("simStep", initialiserParams.motionParameters.simStep);
			simDataPtr->params.emplace("simSamples", static_cast<double>(initialiserParams.motionParameters.simSamples));
			for (auto& jointName : initialiserParams.motionParameters.jointNames) {
				auto& jointLimits = initialiserParams.motionParameters.jointLimits.at(jointName);
				auto randomVector = generateRandomVector(jointLimits);
				simDataPtr->torque.emplace(std::make_pair(jointName, randomVector));
			}
			retVal.emplace_back(simInfo);
		}
		return retVal;
	}
}
