#pragma once

#include "SharedSynchronisation.h"

static bool checkStopFlagAndMaybeWait(std::atomic<bool> & pauseFlag, std::atomic<bool>& stopFlag) {
	while (pauseFlag.load() and !stopFlag.load()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return stopFlag.load();
}
