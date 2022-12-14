// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "GUI/PlotData.h"

PlotData::PlotData() : nextGeneration(0) {

}

void PlotData::updateEAStatistics(DEvA::EAStatistics<Spec> const & eaS, DEvA::EAStatisticsUpdateType updateType) {
	//std::lock_guard<std::mutex> lock(dataMutex);
	//if (eaS.eaProgress.currentGeneration == nextGeneration) {
	//	sliderData.update(eaS);
	//	progressbarData.update(eaS);
	//	if (updateType == DEvA::EAStatisticsUpdateType::Fitness) {
	//		fitnessData.update(eaS);
	//	}
	//	if (updateType == DEvA::EAStatisticsUpdateType::Distance) {
	//		distanceData.update(eaS);
	//	}
	//	if (updateType == DEvA::EAStatisticsUpdateType::Variation) {
	//		variationData.update(eaS);
	//	}
	//	if (updateType == DEvA::EAStatisticsUpdateType::Final) {
	//		++nextGeneration;
	//	}
	//}
}

DEvA::EAProgress PlotData::copyProgress() {
	return m_eaStatistics.eaProgress;
}
