// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Variations/Variations.h"
#include "Wavelet/HaarWavelet.h"

namespace MGEA {
	std::array<size_t, 2> stageToIndices(size_t numJoints, size_t stage) {
		if (0 == stage) {
			return { 0, 1 };
		}
		else if (size_t(1) << (stage + 1) > numJoints) {
			throw(std::logic_error("Stages larger than log2(numGenes)+1 are not possible."));
		}
		else if (size_t(1) << (stage + 1) == numJoints) {
			return { 0, numJoints - 1 };
		}
		else {
			const size_t minIndex(size_t(1) << stage);
			const size_t maxIndex(2 * minIndex - 1);
			return { minIndex,maxIndex };
		}
	}

	SimulationDataPtrs waveletSNV(MotionParameters motionParameters, DEvA::ParameterMap parameters, Spec::IndividualPtrs iptrs) {
		auto parentPtr = iptrs.front();
		auto const & parent = *parentPtr->genotype;

		SimulationDataPtr childDataPtr = std::make_shared<SimulationData>();

		childDataPtr->time = parent.time;
		childDataPtr->params = parent.params;
		childDataPtr->torque = parent.torque;

		std::size_t const simLength = motionParameters.simSamples;
		std::size_t const numJoints = motionParameters.jointNames.size();
		std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
		std::string const& randJointName = motionParameters.jointNames.at(randJointIndex);
		std::size_t const randTimeIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, simLength - 1);
		std::pair<double, double> const& jointLimits = motionParameters.jointLimits.at(randJointName);
		double const randTorque = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(jointLimits.first, jointLimits.second);

		std::size_t i(0);
		for (auto& pair : childDataPtr->torque) {
			if (randJointIndex == i) {
				auto& torqueVector = pair.second;
				torqueVector = haarWaveletEncode(torqueVector);
				torqueVector.at(randTimeIndex) = randTorque;
				torqueVector = haarWaveletDecode(torqueVector);
				break;
			}
			++i;
		}

		return { childDataPtr };
	}
}
