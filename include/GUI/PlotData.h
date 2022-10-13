#pragma once

#include "MotionGeneration/Specification.h"
#include "EvolutionaryAlgorithm.h"

struct DistanceData {
	DistanceData()
		: minimumsOfGenerations({})
		, maximumsOfGenerations({})
		, meansOfGenerations({})
		, minimumOfGenerations(std::numeric_limits<double>::max())
		, maximumOfGenerations(std::numeric_limits<double>::lowest())
		, diffOfGenerations(0)
		, numberOfGenerationsDouble(0.0)
		, numberOfGenerationsInt(0)
		, nextGeneration(0) {};
	std::vector<double> minimumsOfGenerations;
	std::vector<double> maximumsOfGenerations;
	std::vector<double> meansOfGenerations;
	double minimumOfGenerations;
	double maximumOfGenerations;
	double diffOfGenerations;
	double numberOfGenerationsDouble;
	int numberOfGenerationsInt;
	std::size_t nextGeneration;
};

class PlotData {
	public:
		PlotData();
		void update(DEvA::EAStatisticsHistory<Spec> const &);
		DistanceData copyDistanceData();
	private:
		void updateForGeneration(DEvA::EAStatisticsHistory<Spec> const & , std::size_t);
		DEvA::EAStatisticsHistory<Spec> m_eaStatisticsHistory;
		std::size_t nextGeneration;

		void updateDistanceMatrix(DEvA::EAStatistics<Spec> const &, std::size_t);
		DistanceData distanceData;
};
