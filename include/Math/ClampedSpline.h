#pragma once

#include "Common.h"

#include <list>
#include <map>
#include <string>

namespace MGEA {
	struct SplineControlPoint {
		std::size_t index{};
		double value{};
		//NLOHMANN_DEFINE_TYPE_INTRUSIVE(SplineControlPoint, index, value)
	};
	/*inline void from_json(JSON const & j, MGEA::SplineControlPoint & scp) {
		j.at("index").get_to(scp.index);
		j.at("value").get_to(scp.value);
	}*/
	inline void to_json(JSON & j, MGEA::SplineControlPoint const & scp) {
		j = "[" + std::to_string(scp.index) + ", " + std::to_string(scp.value) + "]";
	}

	using SplineControlPoints = std::list<SplineControlPoint>;

	struct ClampedSpline {
		SplineControlPoints controlPoints;

		std::size_t size();
		void addControlPoint(SplineControlPoint);
		bool removeControlPointAt(std::size_t);
		bool removeNthControlPoint(std::size_t);
		DataVector evaluate();
	};

	using ClampedSplineMap = std::map<std::string, ClampedSpline>;
}
