#pragma once

#include "MotionGeneration/Specification.h"

#include <list>
#include <string>
#include <vector>

namespace MGEA {
	void cullPartiallyDominated(std::vector<std::string> metrics, Spec::MetricComparisonMap compMap, Spec::IndividualPtrs & iptrs);
	void onlyPositivesIfThereIsAny(std::string metric, Spec::IndividualPtrs & iptrs);
	void paretoFront(std::vector<std::string> metrics, Spec::IndividualPtrs & iptrs);
	void survivorSelectionOverMetric(std::string metric, Spec::FSurvivorSelection fSelection, Spec::IndividualPtrs & iptrs);
}
