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
		SimulationError
	};
	template <typename T> using Maybe = std::expected<T, ErrorCode>;
}
