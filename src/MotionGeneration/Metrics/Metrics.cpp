// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "MotionGeneration/Metrics/Metrics.h"

#include <any>
#include <cmath>
#include <vector>

namespace MGEA {
	JSON orderedVectorConversion(std::any value) {
		return std::any_cast<MGEA::OrderedVector>(value);
	}

	JSON steppedDoubleConversion(std::any value) {
		return std::any_cast<MGEA::SteppedDouble>(value);
	}

	bool steppedDoubleEquivalence(std::any lhs, std::any rhs) {
		auto lhsPair(std::any_cast<MGEA::SteppedDouble>(lhs));
		auto rhsPair(std::any_cast<MGEA::SteppedDouble>(rhs));
		return (lhsPair.first == rhsPair.first) and (lhsPair.second == rhsPair.second);
	}

	bool steppedDoubleLesser(std::any lhs, std::any rhs) {
		auto lhsPair(std::any_cast<MGEA::SteppedDouble>(lhs));
		auto rhsPair(std::any_cast<MGEA::SteppedDouble>(rhs));
		if (lhsPair.first != rhsPair.first) {
			return lhsPair.first > rhsPair.first;
		}
		return lhsPair.second < rhsPair.second;
	}

	bool steppedDoubleGreater(std::any lhs, std::any rhs) {
		auto lhsPair(std::any_cast<MGEA::SteppedDouble>(lhs));
		auto rhsPair(std::any_cast<MGEA::SteppedDouble>(rhs));
		if (lhsPair.first != rhsPair.first) {
			return lhsPair.first > rhsPair.first;
		}
		return lhsPair.second > rhsPair.second;
	}
}