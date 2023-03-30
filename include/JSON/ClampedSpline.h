#include "DEvA/Error.h"
#include "DEvA/JSON/Common.h"

#include "Math/ClampedSpline.h"
#include "JSON/SplineControlPoint.h"

#include <utility>

template <>
struct nlohmann::adl_serializer<MGEA::ClampedSpline> {
	static MGEA::ClampedSpline from_json(JSON const & j) {
		MGEA::ClampedSpline cs;
		cs.controlPoints = j.get<MGEA::SplineControlPoints>();
		return cs;
	}

	static void to_json(JSON & j, MGEA::ClampedSpline const & cs) {
		j = cs.controlPoints;
	}
};