#pragma once

#include "MotionGeneration/Specification.h"

#include "EvolutionaryAlgorithm.h"

#include <map>
#include <mutex>
#include <string>
#include <vector>

struct EpochProgress {
	EpochProgress();
	std::size_t failed;
	std::size_t evaluated;
	std::size_t total;
	void updateWith(EpochProgress const &);
	bool changed;
};

struct EAProgress {
	EAProgress();
	std::size_t currentGeneration;
	std::size_t numberOfGenerations;
	void updateWith(EAProgress const &);
	bool changed;
};

struct FitnessStatus {
	FitnessStatus();
	std::vector<Spec::Fitness> fitnesses;
	void updateWith(FitnessStatus const &);
	bool changed;
};

//using VariationStatistics = DEvA::VariationStatistics;
struct VariationStatus {
	std::map<std::string, DEvA::VariationInfo<Spec>> variationStatistics;
};

enum StateComponentChanged : std::size_t {
	StateComponentChanged_None = 0,
	StateComponentChanged_EpochProgress = 1,
	StateComponentChanged_EAProgress = 2,
	StateComponentChanged_FitnessStatus = 4,
	StateComponentChanged_VariationStatus = 8
};
class MotionGenerationState {
	public:
		MotionGenerationState();
		void updateWith(EpochProgress const &);
		void updateWith(EAProgress const &);
		void updateWith(FitnessStatus const &);
		void updateWith(MotionGenerationState const &);
		StateComponentChanged changed() const;

		EpochProgress const & epochProgress();
		EAProgress const & eaProgress();
		FitnessStatus const & fitnessStatus();
		//VariationStatus const & variationStatus();
	private:
		mutable std::mutex changeMutex;
		EpochProgress m_epochProgress;
		EAProgress m_eaProgress;
		FitnessStatus m_fitnessStatus;
};
