#pragma once

#include <filesystem>
#include <map>
#include <memory>

#include "SimulationData.h"

class SimulationLog {
	public:
		SimulationLog(std::size_t, std::size_t, std::filesystem::path);

		std::size_t generation() const { return m_gen; };
		std::size_t identifier() const { return m_id; };

		bool inputExists() const { return m_hasInput; };
		bool outputExists() const { return m_hasOutput; };

		SimulationDataPtr loadInput();
		SimulationDataPtr loadOutput();

		SimulationDataPtr createInput(SimulationDataPtr);
		SimulationDataPtr createOutput(SimulationDataPtr);
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
};

using SimulationLogPtr = std::shared_ptr<SimulationLog>;
using SimulationLogPtrMap = std::map<std::size_t, SimulationLogPtr>;
