#include "Logging/SpdlogCommon.h"

#include "spdlog/async.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void initialiseLogger() {
	std::string pattern("[%Y%m%d:%H%M%S.%f][MGEA][%l] %v");
	spdlog::init_thread_pool(8192, 1);
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_pattern(pattern);
	console_sink->set_level(spdlog::level::trace);
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs.txt", true);
	file_sink->set_pattern(pattern);
	file_sink->set_level(spdlog::level::trace);
	std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
	auto logger = std::make_shared<spdlog::async_logger>("MGEALogger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
	logger->flush_on(spdlog::level::trace);
	spdlog::register_logger(logger);
	spdlog::set_default_logger(logger);
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
