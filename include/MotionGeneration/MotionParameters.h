#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

struct MotionParameters {
	double simStart;
	double simStep;
	std::size_t simSamples;
	double simStop() { return simStart + simStep * static_cast<double>(simSamples); };
	int alignment;
	double timeout;
	std::vector<std::string> jointNames;
	std::map<std::string, std::pair<double, double>> jointLimits;
};
