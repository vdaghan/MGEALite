#pragma once

#include "Logging/SpdlogCommon.h"
#include "spdlog/sinks/base_sink.h"

#include <atomic>
#include <list>
#include <mutex>
#include <string>

template <typename Mutex>
class GUILogger : public spdlog::sinks::base_sink<Mutex> {
	protected:
		void sink_it_(spdlog::details::log_msg const & msg) override {
			spdlog::memory_buf_t formatted;
			spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
			std::lock_guard<std::mutex> lock(logReadMutex);
			//logs.push_back(fmt::to_string(formatted));
			logs.emplace_front(fmt::to_string(formatted));
			if (logs.size() > logLength) {
				logs.resize(logLength);
			}
		}
		void flush_() override {} // No flush mechanism for at the moment
	public:
		//TODO: Use something like shared_from_this to get rid of second parameter
		bool addToLogger(std::string loggerName, std::shared_ptr<GUILogger> ptr) {
			//spdlog::get("MGEALogger")->sinks().push_back(guiLoggerPtr);
			auto loggerPtr = spdlog::get(loggerName);
			if (nullptr == loggerPtr) {
				return false;
			}
			auto & sinks = loggerPtr->sinks();
			sinks.push_back(ptr);
			return true;
		}
		void setLogLength(std::size_t newLength) {
			logLength = newLength;
		}
		std::size_t getLogLength() {
			return logLength;
		}
		std::list<std::string> getLogs() {
			std::lock_guard<std::mutex> lock(logReadMutex);
			return logs;
		}
	private:
		std::mutex logReadMutex;
		std::list<std::string> logs;
		std::size_t logLength;
};

#include "spdlog/details/null_mutex.h"
using GUILogger_mt = GUILogger<std::mutex>;
using GUILogger_st = GUILogger<spdlog::details::null_mutex>;
