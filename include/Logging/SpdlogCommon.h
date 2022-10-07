#pragma once

#include <string>

#include "spdlog/spdlog.h"

#include "Logger.h"

#include "Logging/CustomFormatting.h"

void initialiseLogger();
void DEvALoggerCallback(DEvA::LogType, std::string);
