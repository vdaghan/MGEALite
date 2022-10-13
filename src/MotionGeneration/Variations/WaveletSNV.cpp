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

	SimulationDataPtrs waveletSNV(VariationParams variationParameters, SimulationDataPtrs parents) {
		auto const& parent = *parents.front();

		SimulationDataPtr childDataPtr = SimulationDataPtr(new SimulationData());

		childDataPtr->time = parent.time;
		childDataPtr->params = parent.params;
		childDataPtr->torque = parent.torque;

		std::size_t const simLength = variationParameters.motionParameters.simSamples;
		std::size_t const numJoints = variationParameters.motionParameters.jointNames.size();
		std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
		std::string const& randJointName = variationParameters.motionParameters.jointNames.at(randJointIndex);
		std::size_t const randTimeIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, simLength - 1);
		std::pair<double, double> const& jointLimits = variationParameters.motionParameters.jointLimits.at(randJointName);
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
