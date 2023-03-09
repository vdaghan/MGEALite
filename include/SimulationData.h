#pragma once

#include "Common.h"
#include "Metric.h"
#include "MGEAError.h"
#include "Math/ClampedSpline.h"
#include "MotionGeneration/MotionParameters.h"

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

struct SimulationData {
	MGEA::DataVector time;
	std::optional<int> alignment;
	std::optional<double> timeout;
	std::map<std::string, double> params;
	std::map<std::string, double> masses;
	std::map<std::string, double> metadata;
	std::optional<ContactParameters> contacts;
	std::optional<MGEA::ClampedSplineMap> torqueSplines;
	MGEA::VectorMap torque;
	MGEA::VectorMap angles;
	MGEA::VectorMap outputs;
	std::optional<std::string> metrics;
	std::optional<std::string> error;
	bool valid();
	void updateUsing(SimulationData const &);
};

using SimulationDataPtr = std::shared_ptr<SimulationData>;
using MaybeSimulationDataPtr = MGEA::Maybe<SimulationDataPtr>;
using SimulationDataPtrs = std::list<SimulationDataPtr>;
