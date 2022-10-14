#pragma once

#include "MotionGeneration/Specification.h"
#include "EvolutionaryAlgorithm.h"

#include "GUI/GUIData/ProgressbarData.h"
#include "GUI/GUIData/SliderData.h"

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
		void updateEAStatistics(DEvA::EAStatistics<Spec>, DEvA::EAStatisticsUpdateType);

		DEvA::EAProgress copyProgress();
		DistanceData distanceData;
		MGEA::ProgressbarData progressbarData;
		MGEA::SliderData sliderData;
	private:
		DEvA::EAStatistics<Spec> m_eaStatistics;
		std::size_t nextGeneration;

		void updateDistanceMatrix(DEvA::EAStatistics<Spec> const &, DEvA::EAStatisticsUpdateType, std::size_t);
};
