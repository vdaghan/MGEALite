#pragma once

#include <map>
#include <string>
#include <vector>

namespace MGEA {
	using DataVector = std::vector<double>;
	using VectorMap = std::map<std::string, DataVector>;
	using OrderedVector = std::vector<DataVector>;

	std::vector<std::size_t> getExtents(OrderedVector);
	OrderedVector orderedVectorFromVectorMap(VectorMap const &);
	VectorMap vectorMapFromOrderedVector(VectorMap const &, OrderedVector const &);
}
