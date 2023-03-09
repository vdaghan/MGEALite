#include "Math/ClampedSpline.h"

#include <algorithm>
#include <cmath>

namespace MGEA {
	std::size_t ClampedSpline::size() {
		if (controlPoints.empty()) {
			return 0;
		}
		return controlPoints.back().index;
	}

	void ClampedSpline::addControlPoint(SplineControlPoint cP) {
		controlPoints.emplace_back(cP);
		auto indexSort = [](auto const & lhs, auto const & rhs) {
			return lhs.index < rhs.index;
		};
		std::stable_sort(controlPoints.begin(), controlPoints.end(), indexSort);
		auto indexEqual = [](auto const & lhs, auto const & rhs) {
			return lhs.index == rhs.index;
		};
		auto it(std::unique(controlPoints.begin(), controlPoints.end(), indexEqual));
		controlPoints.erase(it, controlPoints.end());
	}
	
	bool ClampedSpline::removeControlPointAt(std::size_t index) {
		auto sameIndex = [index](auto const & controlPoint) {
			return controlPoint.index == index;
		};
		auto it(std::find_if(controlPoints.begin(), controlPoints.end(), sameIndex));
		if (it == controlPoints.end()) {
			return false;
		}
		controlPoints.erase(it);
		return true;
	}

	bool ClampedSpline::removeNthControlPoint(std::size_t index) {
		if (index >= controlPoints.size()) {
			return false;
		}
		auto it(controlPoints.begin());
		std::advance(it, index);
		controlPoints.erase(it);
		return true;
	}

	DataVector ClampedSpline::evaluate() {
		auto const vectorSize(size()+1);
		if (vectorSize < 2) {
			return {};
		}
		DataVector retVal(vectorSize, 0.0);

		for (auto controlPointIt(controlPoints.begin()); std::next(controlPointIt) != controlPoints.end(); ++controlPointIt) {
			auto cP1It(controlPointIt);
			auto cP2It(std::next(controlPointIt));
			auto cP1Index(cP1It->index);
			auto cP2Index(cP2It->index);
			double x1(static_cast<double>(cP1Index));
			double x2(static_cast<double>(cP2Index));
			double y1(cP1It->value);
			double y2(cP2It->value);

			for (std::size_t i(cP1Index); i <= cP2Index; ++i) {
				double x(static_cast<double>(i));
				double dx(x2-x1);
				double a(-2.0 * (y2 - y1) / std::pow(dx, 3.0));
				double b((y2 - y1 - a * std::pow(dx, 3.0)) / std::pow(dx, 2.0));
				retVal[i]
					= a * std::pow(x - x1, 3.0)
					+ b * std::pow(x - x1, 2.0)
					+ y1;
			}
		}

		return retVal;
	};
}
