#pragma once

#include "Common.h"

#include <nlohmann/json.hpp>
using JSON = nlohmann::ordered_json;

#include <variant>

using MGEAMetricVariant = std::variant<
	double,
	MGEA::OrderedVector
>;
using MGEAMetricVariantMap = std::map<std::string, MGEAMetricVariant>;

//void to_json(JSON &, MGEAMetricVariantMap const &);
//void from_json(JSON const &, MGEAMetricVariantMap &);
