#pragma once

#include "MotionGeneration/Specification.h"
#include "EvolutionaryAlgorithm.h"

#include <atomic>
#include <format>

namespace MGEA {
	class SliderData {
		public:
			SliderData() : m_currentGeneration(0) {}

			int & generationIndex(bool snapToLastGeneration) {
				if (snapToLastGeneration) {
					m_generationIndex = currentGeneration();
				}
				return m_generationIndex;
			};
			int currentGeneration() { return static_cast<int>(m_currentGeneration.load()); };

			void update(DEvA::EAStatistics<Spec> const & eaStatistics) {
				auto const & eaProgress(eaStatistics.eaProgress);
				m_currentGeneration.store(eaProgress.currentGeneration);
			}
		private:
			int m_generationIndex;
			std::atomic<std::size_t> m_currentGeneration;
	};
}
