// TODO: Read mapping between json tags and vector indices from a json file
// TODO: Read json input files

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"

#include "mgealite_version.h"
#include "Database.h"
#include "MotionGeneration/MotionGenerator.h"

int main() {
	spdlog::set_pattern("[%Y%m%d:%H:%M:%S.%f][%t][%l] %v");
	spdlog::info("MGEALite version: {}", getMGEALiteVersion());

	MotionGenerator motionGenerator("./data");
	motionGenerator.epoch();

	return 0;
}
