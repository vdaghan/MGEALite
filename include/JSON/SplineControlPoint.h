#pragma once

#include "DEvA/Error.h"
#include "DEvA/JSON/Common.h"

#include "Math/ClampedSpline.h"

#include <utility>

template <>
struct nlohmann::adl_serializer<MGEA::SplineControlPoint> {
	static MGEA::SplineControlPoint from_json(JSON const & j) {
		MGEA::SplineControlPoint scp;
		scp.index = j.at("index").get<std::size_t>();
		scp.value = j.at("value").get<double>();
		return scp;
	}

	static void to_json(JSON & j, MGEA::SplineControlPoint const & scp) {
		j["index"] = scp.index;
		j["value"] = scp.value;
	}
};