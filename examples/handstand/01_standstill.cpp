// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include "MGEA.h"

int main() {
	spdlog::info("DEvA version: {}", getDEvAVersion());
	spdlog::info("MGEALite version: {}", getMGEALiteVersion());

	MotionGenerator motionGenerator("./data", "./01_standstill.json");
	auto compileResult = motionGenerator.compile();
	motionGenerator.lambda = 256;
	auto result = motionGenerator.search(50);

	return 0;
}
