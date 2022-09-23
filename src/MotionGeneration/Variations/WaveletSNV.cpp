#include "MotionGeneration/Variations/WaveletSNV.h"
#include "EvolutionaryAlgorithm.h"

std::array<size_t, 2> stageToIndices(size_t numJoints, size_t stage) {
	if (0 == stage) {
		return {0, 1};
	} else if (size_t(1) << (stage + 1) > numJoints) {
		throw(std::logic_error("Stages larger than log2(numGenes)+1 are not possible."));
	} else if (size_t(1) << (stage + 1) == numJoints) {
		return {0, numJoints - 1};
	} else {
		const size_t minIndex(size_t(1) << stage);
		const size_t maxIndex(2 * minIndex - 1);
		return {minIndex,maxIndex};
	}
}

std::vector<double> haarWaveletEncodeSingle(std::vector<double> const & input) {
	std::array<double, 2> const c = {7.071067811865475E-01, 7.071067811865475E-01};

	size_t const numGenes(input.size());

	size_t m(numGenes);
	std::vector<double> y(input);
	std::vector<double> z(numGenes, 0.0);

	while (2 <= m) {
		m = m / 2;

		for (int i = 0; i < m; i++) {
			z[i] = c[0] * (y[2 * i] + y[2 * i + 1]);
			z[i + m] = c[1] * (y[2 * i] - y[2 * i + 1]);
		}

		for (int i = 0; i < 2 * m; i++) {
			y[i] = z[i];
		}
	}

	return y;
}

std::vector<double> haarWaveletDecodeSingle(std::vector<double> const & input) {
	std::array<double, 2> const c = {7.071067811865475E-01, 7.071067811865475E-01};

	size_t const numGenes(input.size());

	std::vector<double> x(input);
	std::vector<double> z(numGenes, 0.0);

	for (int m = 1; m * 2 <= numGenes; m *= 2) {
		for (int i = 0; i < m; ++i) {
			z[2 * i] = c[0] * (x[i] + x[i + m]);
			z[2 * i + 1] = c[1] * (x[i] - x[i + m]);
		}

		for (int i = 0; i < 2 * m; ++i) {
			x[i] = z[i];
		}
	}

	return x;
}

std::vector<std::vector<double>> haarWaveletEncode(const std::vector<std::vector<double>> & input) {
	std::vector<std::vector<double>> retVal;
	size_t const numChromosomes(input.size());

	for (size_t chromosome(0); chromosome < numChromosomes; ++chromosome) {
		retVal.emplace_back(haarWaveletEncodeSingle(input[chromosome]));
	}

	return retVal;
}

std::vector<std::vector<double>> haarWaveletDecode(const std::vector<std::vector<double>> & input) {
	std::vector<std::vector<double>> retVal;
	size_t const numChromosomes(input.size());

	for (size_t i(0); i < numChromosomes; ++i) {
		retVal.emplace_back(haarWaveletDecodeSingle(input[i]));
	}

	return retVal;
}

SimulationDataPtrs waveletSNV(MotionParameters const & motionParameters, SimulationDataPtrs parents) {
	auto const & parent = *parents.front();

	std::size_t const simLength = motionParameters.simSamples;
	std::size_t const numJoints = motionParameters.jointNames.size();
	if (0 == simLength || 0 == numJoints) {
		return {};
	}

	SimulationDataPtr childDataPtr = SimulationDataPtr(new SimulationData());

	childDataPtr->time = parent.time;
	childDataPtr->params = parent.params;
	childDataPtr->torque = parent.torque;

	std::size_t const randJointIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, numJoints - 1);
	std::string const & randJointName = motionParameters.jointNames.at(randJointIndex);
	std::size_t const randTimeIndex = DEvA::RandomNumberGenerator::get()->getIntBetween<std::size_t>(0, simLength - 1);
	std::pair<double, double> const & jointLimits = motionParameters.jointLimits.at(randJointName);
	double const randTorque = DEvA::RandomNumberGenerator::get()->getRealBetween<double>(jointLimits.first, jointLimits.second);

	std::size_t i(0);
	for (auto & pair : childDataPtr->torque) {
		if (randJointIndex == i) {
			auto & torqueVector = pair.second;
			torqueVector = haarWaveletEncodeSingle(torqueVector);
			torqueVector.at(randTimeIndex) = randTorque;
			torqueVector = haarWaveletDecodeSingle(torqueVector);
			break;
		}
		++i;
	}

	return {childDataPtr};
}
