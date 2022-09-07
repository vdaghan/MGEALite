#pragma once

#include <deque>
#include <filesystem>
#include <map>
#include <memory>

#include "SimulationData.h"
#include "SimulationInfo.h"

class SimulationLog {
	public:
		SimulationLog(SimulationInfo, std::filesystem::path);

		std::size_t generation() const { return m_gen; };
		std::size_t identifier() const { return m_id; };

		SimulationInfo info() const { return SimulationInfo{.generation = m_gen, .identifier = m_id }; };

		bool inputExists();
		bool outputExists();
		bool fitnessExists() { return fitness.has_value(); };

		SimulationDataPtr loadInput();
		SimulationDataPtr loadOutput();
		std::optional<double> loadFitness() { return fitness; };

		SimulationDataPtr createInput(SimulationDataPtr);
		SimulationDataPtr createOutput(SimulationDataPtr);
		double createFitness(double f) { fitness = f; return *fitness; };
	private:
		std::size_t const m_gen;
		std::size_t const m_id;

		std::filesystem::path const m_generationPath;

		std::filesystem::path const m_inputFilePath;
		bool m_hasInput;
		SimulationDataPtr m_input;

		std::filesystem::path const m_outputFilePath;
		bool m_hasOutput;
		SimulationDataPtr m_output;

		bool m_hasFitness;
		std::optional<double> fitness;
};

using SimulationLogPtr = std::shared_ptr<SimulationLog>;
using SimulationLogPtrs = std::deque<SimulationLogPtr>;
