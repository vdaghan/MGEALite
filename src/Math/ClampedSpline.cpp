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
		cleanup();
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

	bool ClampedSpline::removeNthControlPointAndShiftRest(std::size_t index) {
		if (index >= controlPoints.size()) {
			return false;
		}
		auto it(controlPoints.begin());
		std::advance(it, index);
		while (std::next(it) != controlPoints.end()) {
			auto nextIt(std::next(it));
			it->value = nextIt->value;
			it = nextIt;
		}
		cleanup();
		return true;
	}

	bool ClampedSpline::removeTimePointAndShiftRest(std::size_t index) {
		auto maxTimepoint(size());
		if (index >= maxTimepoint) {
			return false;
		}
		auto it(controlPoints.begin());
		auto endIt(controlPoints.end());
		while (it->index < index and std::next(it)->index <= index) {
			++it;
		}
		if (it->index != index) {
			++it;
		}
		while (it != controlPoints.end() and it->index != maxTimepoint) {
			--(it->index);
			++it;
		}
		cleanup();
		return true;
	}

	std::string ClampedSpline::str() {
		std::string retVal("[ ");
		for (auto & controlPoint : controlPoints) {
			retVal += "(" + std::to_string(controlPoint.index) + ", " + std::to_string(controlPoint.value) + ") ";
		}
		retVal += "]";
		return retVal;
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
				retVal[i] = a * std::pow(x - x1, 3.0)
						  + b * std::pow(x - x1, 2.0)
						  + y1;
			}
		}

		return retVal;
	};
	
	void ClampedSpline::cleanup() {
		auto indexSort = [](auto const & lhs, auto const & rhs) {
			return lhs.index < rhs.index;
		};
		std::stable_sort(controlPoints.begin(), controlPoints.end(), indexSort);

		if (controlPoints.size() >= 3) {
			for (auto it(std::next(controlPoints.begin())); std::next(it) != controlPoints.end(); ++it) {
				auto prevIt(std::prev(it));
				auto nextIt(std::next(it));
				if (prevIt->value == it->value and it->value == nextIt->value) {
					it = controlPoints.erase(it);
				}
			}
		}

		if (controlPoints.size() >= 2) {
			auto it(controlPoints.begin());
			auto nextIt(std::next(it));
			while (nextIt != controlPoints.end()) {
				if (it->index == nextIt->index) {
					it = controlPoints.erase(it);
				} else {
					++it;
				}
				nextIt = std::next(it);
			}
		}
	}
}
