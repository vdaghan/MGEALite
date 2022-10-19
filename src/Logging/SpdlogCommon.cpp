#include "Logging/SpdlogCommon.h"

#include "spdlog/async.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::async_logger> logger;

void initialiseLogger() {
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs.txt", true);
	std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };

	std::string pattern("[%Y%m%d:%H%M%S.%f][MGEA][%l] %v");
	spdlog::default_logger()->sinks().push_back(console_sink);
	spdlog::default_logger()->sinks().push_back(file_sink);
	spdlog::default_logger()->set_pattern(pattern);
	spdlog::default_logger()->set_level(spdlog::level::trace);
	spdlog::default_logger()->flush_on(spdlog::level::trace);
}

void DEvALoggerCallback(DEvA::LogType logType, std::string message) {
	if (DEvA::LogType::Trace == logType) {
		spdlog::trace(message);
	} else if (DEvA::LogType::Debug == logType) {
		spdlog::debug(message);
	} else if (DEvA::LogType::Info == logType) {
		spdlog::info(message);
	} else if (DEvA::LogType::Warning == logType) {
		spdlog::warn(message);
	} else if (DEvA::LogType::Error == logType) {
		spdlog::error(message);
	} else if (DEvA::LogType::Critical == logType) {
		spdlog::critical(message);
	}
}
