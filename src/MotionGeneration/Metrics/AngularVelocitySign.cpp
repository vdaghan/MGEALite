// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Metrics/Metrics.h"

#include <cmath>
#include <vector>

namespace MGEA {
	std::any angularVelocitySign(DEvA::ParameterMap parameterMap, Spec::IndividualPtr iptr) {
		auto & simDataPtr(*iptr->maybePhenotype);
		VectorMap const & angles(simDataPtr->angles);

		OrderedVector anglesVector(orderedVectorFromVectorMap(angles));
		auto angleExtents(getExtents(anglesVector));
		if (angleExtents.empty() or 0 == angleExtents[0]) {
			return {};
		}

		OrderedVector differences{};
		for (auto& angleVector : anglesVector) {
			differences.emplace_back();
			auto& differenceVector = differences.back();
			for (std::size_t i(0); i + 1 < angleExtents[1]; ++i) {
				auto difference(angleVector[i + 1] - angleVector[i]);
				differenceVector.emplace_back(difference);
			}
		}

		auto differenceExtents(getExtents(differences));
		if (differenceExtents.empty() or 0 == differenceExtents[0]) {
			return {};
		}
		OrderedVector avsv{};
		for (std::size_t i(0); i != differenceExtents[1]; ++i) {
			DataVector sign(differenceExtents[0]);
			for (std::size_t j(0); j != differenceExtents[0]; ++j) {
				if (differences[j][i] == 0.0) {
					sign[j] = 0.0;
				}
				else {
					sign[j] = std::copysign(1.0, differences[j][i]);
				}
			}
			avsv.emplace_back(sign);
		}

		auto avsvExtents(getExtents(avsv));
		if (avsvExtents.empty() or 0 == avsvExtents[0]) {
			return {};
		}
		OrderedVector retVal{};
		std::size_t i(0);
		while (i != avsvExtents[0]) {
			std::size_t j(i + 1);
			while (j < avsvExtents[0]) {
				bool equal(true);
				for (std::size_t k(0); k != avsvExtents[1]; ++k) {
					if (avsv[i][k] != avsv[j][k]) {
						equal = false;
						break;
					}
				}
				if (!equal) {
					break;
				}
				++j;
			}
			retVal.emplace_back(avsv[i]);
			i = j;
		}

		return retVal;
	}

	bool angularVelocitySignEquivalent(std::any lhs, std::any rhs) {
		MGEA::OrderedVector lhsAngularVelocitySign(std::any_cast<MGEA::OrderedVector>(lhs));
		MGEA::OrderedVector rhsAngularVelocitySign(std::any_cast<MGEA::OrderedVector>(rhs));
		std::size_t minVectorSize(std::min(lhsAngularVelocitySign.size(), rhsAngularVelocitySign.size()));
		for (std::size_t i(0); i != minVectorSize; ++i) {
			std::size_t numAngles(lhsAngularVelocitySign.at(i).size());
			if (numAngles != rhsAngularVelocitySign.at(i).size()) {
				throw;
			}
			for (std::size_t j(0); j != numAngles; ++j) {
				bool equalValue(lhsAngularVelocitySign.at(i).at(j) == rhsAngularVelocitySign.at(i).at(j));
				bool lhsIsZero(lhsAngularVelocitySign.at(i).at(j) == 0);
				bool rhsIsZero(rhsAngularVelocitySign.at(i).at(j) == 0);
				if (not (equalValue or lhsIsZero or rhsIsZero)) {
					return false;
				}
			}
		}
		return true;
	}
}
