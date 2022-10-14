#pragma once

#include "MotionGeneration/Specification.h"
#include "EvolutionaryAlgorithm.h"

#include <mutex>
#include <vector>

namespace MGEA {
	class DistanceData {
		public:
			DistanceData()
				: minimumsOfGenerations({})
				, maximumsOfGenerations({})
				, meansOfGenerations({})
				, minimumOfGenerations(std::numeric_limits<double>::max())
				, maximumOfGenerations(std::numeric_limits<double>::lowest())
				, diffOfGenerations(0)
				, numberOfGenerationsDouble(0.0)
				, numberOfGenerationsInt(0)
				, nextGeneration(0) {
			};

			std::vector<double> minimumsOfGenerations;
			std::vector<double> maximumsOfGenerations;
			std::vector<double> meansOfGenerations;
			double minimumOfGenerations;
			double maximumOfGenerations;
			double diffOfGenerations;
			double numberOfGenerationsDouble;
			int numberOfGenerationsInt;
			std::size_t nextGeneration;

			void update(DEvA::EAStatistics<Spec> const & eaStatistics) {
				auto const & distanceMatrix = eaStatistics.distanceMatrix;

				double numberOfIndividuals(static_cast<double>(distanceMatrix.size()));
				std::size_t minimumOfGenerationSizeT(std::numeric_limits<std::size_t>::max());
				std::size_t maximumOfGenerationSizeT(std::numeric_limits<std::size_t>::lowest());
				std::size_t totalOfGenerationSizeT(0);
				for (auto & aa : distanceMatrix) {
					auto & bb(aa.second);
					for (auto & cc : bb) {
						auto & dd(cc.second);
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

				minimumsOfGenerations.push_back(minimumOfGeneration);
				maximumsOfGenerations.push_back(maximumOfGeneration);
				meansOfGenerations.push_back(meanOfGeneration);
				minimumOfGenerations = std::min(minimumOfGenerations, minimumOfGeneration);
				maximumOfGenerations = std::max(maximumOfGenerations, maximumOfGeneration);
				diffOfGenerations = maximumOfGenerations - minimumOfGenerations;
				numberOfGenerationsDouble = static_cast<double>(nextGeneration + 1);
				numberOfGenerationsInt = static_cast<int>(nextGeneration + 1);
				++nextGeneration;
			}
		private:
			std::mutex mtx;
	};
};
