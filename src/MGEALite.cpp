#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "spdlog/async.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "EvolutionaryAlgorithm.h"
#include <DTimer/DTimer.h>

#include "mgealite_version.h"
#include "Database.h"
#include "MotionGeneration/MotionGenerator.h"

int main() {
	std::string pattern("[%Y%m%d:%H%M%S.%f][MGEA][%l] %v");
	spdlog::init_thread_pool(8192, 1);
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_pattern(pattern);
	console_sink->set_level(spdlog::level::trace);
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs.txt", true);
	file_sink->set_pattern(pattern);
	file_sink->set_level(spdlog::level::trace);
	std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
	auto logger = std::make_shared<spdlog::async_logger>("loggername", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
	logger->flush_on(spdlog::level::trace);
	spdlog::register_logger(logger);
	spdlog::set_default_logger(logger);
	spdlog::info("MGEALite version: {}", getMGEALiteVersion());

	MotionParameters motionParameters;
	motionParameters.simStart = 0.0;
	motionParameters.simStep = 0.01;
	motionParameters.simSamples = 256;
	motionParameters.alignment = -1;
	motionParameters.timeout = 300.0;
	motionParameters.jointNames.push_back("wrist");
	motionParameters.jointNames.push_back("shoulder");
	motionParameters.jointNames.push_back("hip");
	motionParameters.jointNames.push_back("ankle");
	motionParameters.jointLimits.emplace(std::make_pair("wrist", std::make_pair(-30.0, 30.0)));
	motionParameters.jointLimits.emplace(std::make_pair("shoulder", std::make_pair(-200.0, 200.0)));
	motionParameters.jointLimits.emplace(std::make_pair("hip", std::make_pair(-320.0, 500.0))); // https://bmcsportsscimedrehabil.biomedcentral.com/articles/10.1186/s13102-022-00401-9/figures/1
	motionParameters.jointLimits.emplace(std::make_pair("ankle", std::make_pair(-70.0, 200.0))); // https://bmcsportsscimedrehabil.biomedcentral.com/articles/10.1186/s13102-022-00401-9/figures/1
	motionParameters.contactParameters = bodyGroundContactParameters();

	MotionGenerator motionGenerator("./data", motionParameters);
	auto result = motionGenerator.search(250);
	if (DEvA::StepResult::Convergence == result) {
		spdlog::info("Search converged.");
	} else if (DEvA::StepResult::Inconclusive == result) {
		spdlog::info("Search inconclusive.");
	} else if (DEvA::StepResult::StepCount == result) {
		spdlog::info("Step limit reached.");
	}

	spdlog::info("\n{}", DTimer::print());
	return 0;
}
