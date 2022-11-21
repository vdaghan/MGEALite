#pragma once

#include "MotionGeneration/Specification.h"
#include "DEvA/EvolutionaryAlgorithm.h"

#include <list>
#include <mutex>
#include <vector>

namespace MGEA {
	struct IndividualDistanceRange {
		auto operator<=>(IndividualDistanceRange const&) const = default;
		Spec::Distance minimum{};
		Spec::Distance maximum{};
	};
	using DistanceRanges = std::list<IndividualDistanceRange>;
	class DistanceData {
		public:
			//DistanceData()
			//	: minimumsOfGenerations({})
			//	, maximumsOfGenerations({})
			//	, meansOfGenerations({})
			//	, minimumOfGenerations(std::numeric_limits<double>::max())
			//	, maximumOfGenerations(std::numeric_limits<double>::lowest())
			//	, diffOfGenerations(0)
			//	, numberOfGenerationsDouble(0.0)
			//	, numberOfGenerationsInt(0)
			//	, nextGeneration(0) {
			//};

			std::vector<double> minimumsOfGenerations{};
			std::vector<double> maximumsOfGenerations{};
			std::vector<double> meansOfGenerations{};
			double minimumOfGenerations{};
			double maximumOfGenerations{};
			double diffOfGenerations{};
			double numberOfGenerationsDouble{};
			int numberOfGenerationsInt{};

			std::vector<double> minimumsOfIndividuals{};
			std::vector<double> maximumsOfIndividuals{};
			double minimumOfIndividuals{};
			double maximumOfIndividuals{};
			double diffOfIndividuals{};
			std::size_t numberOfIndividuals{};

			std::size_t nextGeneration{};

			void update(DEvA::EAStatistics<Spec> const & eaStatistics) {
				auto const & distanceMatrix = eaStatistics.distanceMatrix;

				double numberOfIndividuals(static_cast<double>(distanceMatrix.size()));
				Spec::Distance minimumOfGenerationSizeT(std::numeric_limits<double>::max());
				Spec::Distance maximumOfGenerationSizeT(std::numeric_limits<double>::lowest());
				Spec::Distance totalOfGenerationSizeT(0);
				for (auto& id1Pair : distanceMatrix) {
					auto& id1(id1Pair.first);
					auto& id1Distances(id1Pair.second);
					for (auto& id2Pair : id1Distances) {
						auto& id2(id2Pair.first);
						if (id1 == id2) [[unlikely]] {
							continue;
						}
						auto& distance(id2Pair.second);
						minimumOfGenerationSizeT = std::min(distance, minimumOfGenerationSizeT);
						maximumOfGenerationSizeT = std::max(distance, maximumOfGenerationSizeT);
						totalOfGenerationSizeT += distance;
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

				DistanceRanges distanceRanges;
				for (auto & id1Pair : distanceMatrix) {
					auto & id1(id1Pair.first);
					auto & id1Distances(id1Pair.second);
					distanceRanges.push_back({});
					distanceRanges.back().minimum = std::numeric_limits<double>::max();
					distanceRanges.back().maximum = std::numeric_limits<double>::lowest();
					for (auto& id2Pair : id1Distances) {
						auto& id2(id2Pair.first);
						if (id1 == id2) [[unlikely]] {
							continue;
						}
						auto& distance(id2Pair.second);
						distanceRanges.back().minimum = std::min(distanceRanges.back().minimum, distance);
						distanceRanges.back().maximum = std::max(distanceRanges.back().maximum, distance);
					}
				}
				distanceRanges.sort([](auto const & dr1, auto const & dr2) { return dr1 > dr2; });
				this->numberOfIndividuals = 0;
				minimumsOfIndividuals.clear();
				maximumsOfIndividuals.clear();
				minimumOfIndividuals = std::numeric_limits<double>::max();
				maximumOfIndividuals = std::numeric_limits<double>::lowest();
				for (auto & distanceRange : distanceRanges) {
					minimumsOfIndividuals.push_back(static_cast<double>(distanceRange.minimum));
					maximumsOfIndividuals.push_back(static_cast<double>(distanceRange.maximum));
					minimumOfIndividuals = std::min(minimumOfIndividuals, static_cast<double>(distanceRange.minimum));
					maximumOfIndividuals = std::max(maximumOfIndividuals, static_cast<double>(distanceRange.maximum));
					++this->numberOfIndividuals;
				}
				diffOfIndividuals = maximumOfIndividuals - minimumOfIndividuals;

				++nextGeneration;
			}
		private:
			std::mutex mtx;
	};
};
