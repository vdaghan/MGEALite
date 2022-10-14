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

			std::string text() { std::lock_guard<std::mutex> lock(mtx); return m_text; };
			float fraction() { std::lock_guard<std::mutex> lock(mtx); return m_fraction; };

			void update(DEvA::EAStatistics<Spec> const & eaStatistics) {
				auto const & eaProgress(eaStatistics.eaProgress);
				auto const & individuals(eaProgress.numberOfIndividualsInGeneration);
				std::lock_guard<std::mutex> lock(mtx);
				if (0 == individuals) {
					m_fraction = 0.0;
					m_text = "N/A";
				} else {
					auto const & transformedIndividuals(eaProgress.numberOfTransformedIndividualsInGeneration);
					auto const & evaluatedIndividuals(eaProgress.numberOfEvaluatedIndividualsInGeneration);
					float individualsFloat(static_cast<float>(individuals));
					if (0 == evaluatedIndividuals) {
						m_fraction = static_cast<float>(transformedIndividuals) / individualsFloat;
						m_text = std::format("{:.2f}% ({}/{}) Transformed", 100.0 * m_fraction, transformedIndividuals, individuals);
					} else {
						m_fraction = static_cast<float>(evaluatedIndividuals) / individualsFloat;
						m_text = std::format("{:.2f}% ({}/{}) Evaluated", 100.0 * m_fraction, evaluatedIndividuals, individuals);
					}
				}
			}
		private:
			std::mutex mtx;
			float m_fraction;
			std::string m_text;
	};
}
