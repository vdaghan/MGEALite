#include "GUI/PlotData.h"

PlotData::PlotData() : nextGeneration(0) {

}

void PlotData::updateEAStatistics(DEvA::EAStatistics<Spec> eaS, DEvA::EAStatisticsUpdateType updateType) {
	if (eaS.eaProgress.currentGeneration == nextGeneration) {
		sliderData.update(eaS);
		progressbarData.update(eaS);
		if (updateType == DEvA::EAStatisticsUpdateType::Fitness) {
			fitnessData.update(eaS);
		}
		if (updateType == DEvA::EAStatisticsUpdateType::Distance) {
			distanceData.update(eaS);
		}
		if (updateType == DEvA::EAStatisticsUpdateType::Variation) {
			variationData.update(eaS);
		}
		if (updateType == DEvA::EAStatisticsUpdateType::Final) {
			++nextGeneration;
		}
	}
}

DEvA::EAProgress PlotData::copyProgress() {
	return m_eaStatistics.eaProgress;
}
