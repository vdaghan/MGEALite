// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Common.h"

namespace MGEA {
	std::vector<std::size_t> getExtents(std::vector<std::vector<double>> matrix) {
		if (matrix.empty()) {
			return {0};
		}
		std::size_t numVectors(matrix.size());

		std::size_t minVectorSize(std::numeric_limits<std::size_t>::max());
		std::size_t maxVectorSize(std::numeric_limits<std::size_t>::lowest());
		for (auto & vector : matrix) {
			minVectorSize = std::min(minVectorSize, vector.size());
			maxVectorSize = std::max(maxVectorSize, vector.size());
		}

		if (minVectorSize != maxVectorSize) {
			return {};
		}

		return { numVectors, minVectorSize };
	}

	std::vector<std::vector<double>> orderedVectorFromVectorMap(VectorMap const & vectorMap) {
		std::vector<std::vector<double>> retVal{};
		for (auto const & vectorPair : vectorMap) {
			auto const & vectorValue(vectorPair.second);
			retVal.push_back(vectorValue);
		}
		return retVal;
	}

	//VectorMap vectorMapFromOrderedVector(VectorMap const & like, std::vector<std::vector<double>> const & orderedVector) {
	//	VectorMap vectorMap{};
	//	for ()
	//}
}
