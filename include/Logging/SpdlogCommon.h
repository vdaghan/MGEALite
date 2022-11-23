#pragma once

#include <string>

#include "spdlog/async.h"
#include "spdlog/spdlog.h"

#include "DEvA/Logger.h"

#include "Logging/CustomFormatting.h"

extern std::shared_ptr<spdlog::async_logger> logger;
void initialiseLogger();
void DEvALoggerCallback(DEvA::LogType, std::string const &);
