// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <chrono>
#include <iostream>
#include <fstream>
#include <numbers>
#include <string>
#include <thread>
#include <vector>

#include "MGEA.h"

int main() {
	MotionGenerator motionGenerator("./data", "./00_barHandstand.json");

	std::vector<std::string> paretoMetrics{ "comheight", "rotation" };
	Spec::BPSurvivorSelection::Parametrisable combinedSurvivorSelectorLambda = [=](DEvA::ParameterMap parameters, Spec::IndividualPtrs iptrs) {
		iptrs = MGEA::cullEquals({}, iptrs);
		JSON j(paretoMetrics);
		iptrs = MGEA::paretoFront(DEvA::ParameterMap({ {"metrics", j} }), iptrs);
		return iptrs;
	};
	auto computeCOMHeightLambda = [&](DEvA::ParameterMap pMap, Spec::IndividualPtr iptr) {
		auto & simDataPtr(*iptr->maybePhenotype);
		double timeStep = motionGenerator.motionParameters.simStep;
		auto & comZ = simDataPtr->outputs.at("centerOfMassZ");
		double comZSum = std::accumulate(comZ.begin(), comZ.end(), 0.0);
		return comZSum * timeStep / motionGenerator.motionParameters.simStop();
	};
	auto computeRotationLambda = [&](DEvA::ParameterMap pMap, Spec::IndividualPtr iptr) {
		auto & simDataPtr(*iptr->maybePhenotype);
		double timeStep = motionGenerator.motionParameters.simStep;
		auto & wristAngle = simDataPtr->angles.at("wrist");
		//double angleSum = std::accumulate(wristAngle.begin(), wristAngle.end(), 0.0);
		return wristAngle.back();
		//return angleSum;
	};

	motionGenerator.functions.survivorSelection.defineParametrised("Combined", combinedSurvivorSelectorLambda, {});
	motionGenerator.functions.survivorSelection.compile("CombinedCompiled", { "Combined" });
	motionGenerator.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("computeCOMHeight", computeCOMHeightLambda));
	motionGenerator.metricFunctors.computeFromIndividualPtrFunctions.emplace(std::pair("computeRotation", computeRotationLambda));

	auto compileResult = motionGenerator.compile();
	motionGenerator.lambda = 256;
	auto result = motionGenerator.search(1024);

	return 0;
}
