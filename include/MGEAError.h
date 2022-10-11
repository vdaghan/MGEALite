#pragma once

#include <expected>

namespace MGEA {
	enum class ErrorCode {
		OK,
		Fail,
		ErrorReadingFile,
		FileNotFound,
		NoInput,
		NoOutput,
		SimulationError,
		Stopped
	};
	template <typename T> using Maybe = std::expected<T, ErrorCode>;
}
