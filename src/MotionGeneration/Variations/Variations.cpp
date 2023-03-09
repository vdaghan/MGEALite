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
		double meanToNearestExtrema(std::min(mean - min, max - mean));
		//double threeSigma(std::max((max - min) / 100.0, meanToNearestExtrema));
		//std::normal_distribution<double> dist(mean, meanToNearestExtrema / 3.0);
		std::normal_distribution<double> dist(mean, 10.0);
		double randomValue(dist(DEvA::RandomNumberGenerator::get()->generator));
		if (randomValue < min) {
			randomValue = min;
		} else if (randomValue > max) {
			randomValue = max;
		}
		return randomValue;
	}
}