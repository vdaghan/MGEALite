#pragma once

#include "EvolutionaryAlgorithm.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include <spdlog/fmt/bundled/format.h>

template<> struct fmt::formatter<DEvA::IndividualIdentifier> {
	constexpr auto parse(format_parse_context & ctx) -> decltype(ctx.begin()) {
		return ctx.end();
	}
	template <typename FormatContext>
	auto format(DEvA::IndividualIdentifier const & input, FormatContext & ctx) -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "({}, {})", input.generation, input.identifier);
	}
};
