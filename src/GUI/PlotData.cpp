#include "GUI/PlotData.h"

PlotData::PlotData() : nextGeneration(0) {

}

void PlotData::updateEAStatistics(DEvA::EAStatistics<Spec> eaS, DEvA::EAStatisticsUpdateType updateType) {
	if (eaS.eaProgress.currentGeneration == nextGeneration) {
		sliderData.update(eaS);
		progressbarData.update(eaS);
		updateDistanceMatrix(eaS, updateType, eaS.eaProgress.currentGeneration);
		if (updateType == DEvA::EAStatisticsUpdateType::Final) {
			++nextGeneration;
		}
	}
}

DEvA::EAProgress PlotData::copyProgress() {
	return m_eaStatistics.eaProgress;
}

void PlotData::updateDistanceMatrix(DEvA::EAStatistics<Spec> const & eaStatistics, DEvA::EAStatisticsUpdateType updateType, std::size_t generation) {
	if (DEvA::EAStatisticsUpdateType::Distance != updateType) {
		return;
	}
	auto const & distanceMatrix = eaStatistics.distanceMatrix;

	double numberOfIndividuals(static_cast<double>(distanceMatrix.size()));
	std::size_t minimumOfGenerationSizeT(std::numeric_limits<std::size_t>::max());
	std::size_t maximumOfGenerationSizeT(std::numeric_limits<std::size_t>::lowest());
	std::size_t totalOfGenerationSizeT(0);
	for (auto& aa : distanceMatrix) {
		auto& bb(aa.second);
		for (auto& cc : bb) {
			auto& dd(cc.second);
			minimumOfGenerationSizeT = std::min(dd, minimumOfGenerationSizeT);
			maximumOfGenerationSizeT = std::max(dd, maximumOfGenerationSizeT);
			totalOfGenerationSizeT += dd;
		}
	}
	totalOfGenerationSizeT /= 2;
	double totalOfGeneration(static_cast<double>(totalOfGenerationSizeT));
	double minimumOfGeneration(static_cast<double>(minimumOfGenerationSizeT));
	double maximumOfGeneration(static_cast<double>(maximumOfGenerationSizeT));
	double meanOfGeneration(totalOfGeneration / (std::pow(numberOfIndividuals, 2)));

	distanceData.minimumsOfGenerations.push_back(minimumOfGeneration);
	distanceData.maximumsOfGenerations.push_back(maximumOfGeneration);
	distanceData.meansOfGenerations.push_back(meanOfGeneration);
	distanceData.minimumOfGenerations = std::min(distanceData.minimumOfGenerations, minimumOfGeneration);
	distanceData.maximumOfGenerations = std::max(distanceData.maximumOfGenerations, maximumOfGeneration);
	distanceData.diffOfGenerations = distanceData.maximumOfGenerations - distanceData.minimumOfGenerations;
	distanceData.numberOfGenerationsDouble = static_cast<double>(nextGeneration+1);
	distanceData.numberOfGenerationsInt = static_cast<int>(nextGeneration+1);
}
