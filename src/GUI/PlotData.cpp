#include "GUI/PlotData.h"

PlotData::PlotData() : nextGeneration(0) {

}

void PlotData::update(DEvA::EAStatisticsHistory<Spec> const & eaStatisticsHistory) {
	std::size_t const newHistorySize(eaStatisticsHistory.size());
	bool updated(false);
	for (std::size_t gen(nextGeneration); gen < newHistorySize; ++gen) {
		updateForGeneration(eaStatisticsHistory, gen);
		++nextGeneration;
	}
}

DistanceData PlotData::copyDistanceData() {
	return distanceData;
}

void PlotData::updateForGeneration(DEvA::EAStatisticsHistory<Spec> const & eaStatisticsHistory, std::size_t generation) {
	m_eaStatisticsHistory.push_back(eaStatisticsHistory[generation]);
	auto const & eaStatistics(m_eaStatisticsHistory[generation]);

	updateDistanceMatrix(eaStatistics, generation);
}

void PlotData::updateDistanceMatrix(DEvA::EAStatistics<Spec> const & eaStatistics, std::size_t generation) {
	auto const & distanceMatrix = eaStatistics.distanceMatrix;
	if (distanceData.nextGeneration != generation) {
		throw;
	}

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
	++distanceData.nextGeneration;
	distanceData.numberOfGenerationsDouble = static_cast<double>(distanceData.nextGeneration);
	distanceData.numberOfGenerationsInt = static_cast<int>(distanceData.nextGeneration);
}
