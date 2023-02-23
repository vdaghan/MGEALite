// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Initialisers/Initialisers.h"

namespace MGEA {
	SimulationDataPtrs genesisRandom(MotionParameters motionParameters, DEvA::ParameterMap parameters) {
		SimulationDataPtrs simDataPtrs;

		std::size_t numIndividuals(parameters.at("N").get<std::size_t>());
		auto generateRandomVector = [&](std::pair<double, double> limits) -> std::vector<double> {
			std::vector<double> retVal(motionParameters.simSamples);
			auto vectorGenerator = [&]() {
				return DEvA::RandomNumberGenerator::get()->getRealBetween<double>(limits.first, limits.second);
			};
			std::generate(retVal.begin(), retVal.end(), vectorGenerator);
			return retVal;
		};

		for (size_t n(0); n != numIndividuals; ++n) {
			auto simDataPtr = std::make_shared<SimulationData>();
			for (auto& jointName : motionParameters.jointNames) {
				auto& jointLimits = motionParameters.jointLimits.at(jointName);
				auto randomVector = generateRandomVector(jointLimits);
				simDataPtr->torque.emplace(std::make_pair(jointName, randomVector));
			}
			simDataPtrs.emplace_back(simDataPtr);
		}
		return simDataPtrs;
	}
}
