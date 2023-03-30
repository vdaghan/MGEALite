// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Common.h"
#include "MotionGeneration/Specification.h"
#include "MotionGeneration/Variations/Variations.h"

#include <any>
#include <cmath>
#include <vector>

namespace MGEA {
	double generateCenteredRandomDouble(double min, double mean, double max) {
		//std::normal_distribution<double> dist(mean, ((max - min) / 10.0)/6.0);
		//std::uniform_real_distribution<double> dist(min, max);
		//double randomValue(dist(DEvA::RandomNumberGenerator::get()->generator));
		double randomValue(DEvA::RandomNumberGenerator::get()->getRealBetween<double>(min, max));
		if (randomValue < min) {
			randomValue = min;
		} else if (randomValue > max) {
			randomValue = max;
		}
		return randomValue;
	}
}