#pragma once

#include "MotionGeneration/Specification.h"
#include "EvolutionaryAlgorithm.h"

#include <format>
#include <mutex>
#include <string>

namespace MGEA {
	class ProgressbarData {
		public:
			ProgressbarData() : m_text(), m_fraction(0.0) {}

			std::string text() { auto localLock(lock()); return m_text; };
			float fraction() { auto localLock(lock()); return m_fraction; };

			std::unique_lock<std::mutex> lock() const { return std::unique_lock(accessMutex); };
			void update(DEvA::EAStatistics<Spec> const & eaStatistics) {
				auto localLock(lock());
				auto const & eaProgress(eaStatistics.eaProgress);
				auto const & eaStage(eaProgress.eaStage);
				auto const & individuals(eaProgress.numberOfNewIndividualsInGeneration);
				if (DEvA::EAStage::Create == eaStage) {
					m_fraction = 0.0;
					m_text = "Creating Individuals...";
				} else if (DEvA::EAStage::Transform == eaStage) {
					float individualsFloat(static_cast<float>(individuals));
					auto const & transformedIndividuals(eaProgress.numberOfTransformedIndividualsInGeneration);
					m_fraction = static_cast<float>(transformedIndividuals) / individualsFloat;
					m_text = std::format("{:.2f}% ({}/{}) Transformed", 100.0 * m_fraction, transformedIndividuals, individuals);
				} else if (DEvA::EAStage::Evaluate == eaStage) {
					float individualsFloat(static_cast<float>(individuals));
					auto const & evaluatedIndividuals(eaProgress.numberOfEvaluatedIndividualsInGeneration);
					m_fraction = static_cast<float>(evaluatedIndividuals) / individualsFloat;
					m_text = std::format("{:.2f}% ({}/{}) Evaluated", 100.0 * m_fraction, evaluatedIndividuals, individuals);
				} else if (DEvA::EAStage::Distance == eaStage) {
					m_fraction = 0.0;
					m_text = "Computing Distances...";
				} else if (DEvA::EAStage::End == eaStage) {
					m_fraction = 0.0;
					m_text = "End of generation.";
				} else {
					m_fraction = 0.0;
					m_text = "N/A";
				}
			}
		private:
			mutable std::mutex accessMutex;
			float m_fraction;
			std::string m_text;
	};
}
