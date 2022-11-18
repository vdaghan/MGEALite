#include "Metric.h"

//void to_json(JSON & j, MGEAMetricVariantMap const & mVM) {
//	for (auto & metricVariantPair : mVM) {
//		auto & metricVariantName(metricVariantPair.first);
//		auto & metricVariantValue(metricVariantPair.second);
//		if (std::holds_alternative<double>(metricVariantValue)) {
//			j[metricVariantName] = std::get<double>(metricVariantValue);
//		}
//	}
//}
//
//static void from_json(JSON const & j, MGEAMetricVariantMap & m) {
//	if (j.contains("metrics")) {
//		auto & metrics = j["metrics"];
//		for (auto metricIt = metrics.begin(); metricIt != metrics.end(); ++metricIt) {
//			std::string metricKey = metricIt.key();
//			auto & metricValue = metricIt.value();
//			m.emplace(std::make_pair(metricKey, metricValue));
//		}
//	}
//}