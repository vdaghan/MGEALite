#pragma once

#include "MotionGeneration/Specification.h"
#include "EvolutionaryAlgorithm.h"

#include "GUI/GUIData/DistanceData.h"
#include "GUI/GUIData/FitnessData.h"
#include "GUI/GUIData/ProgressbarData.h"
#include "GUI/GUIData/SliderData.h"
#include "GUI/GUIData/VariationData.h"

class PlotData {
	public:
		PlotData();
		void updateEAStatistics(DEvA::EAStatistics<Spec> const &, DEvA::EAStatisticsUpdateType);

		DEvA::EAProgress copyProgress();
		MGEA::DistanceData distanceData;
		MGEA::FitnessData fitnessData;
		MGEA::ProgressbarData progressbarData;
		MGEA::SliderData sliderData;
		MGEA::VariationData variationData;
	private:
		std::mutex dataMutex;
		DEvA::EAStatistics<Spec> m_eaStatistics;
		std::size_t nextGeneration;

		void updateDistanceMatrix(DEvA::EAStatistics<Spec> const &, DEvA::EAStatisticsUpdateType, std::size_t);
};
