#include "SimulationLog.h"

#include "spdlog/spdlog.h"

SimulationLog::SimulationLog(std::size_t gen, std::size_t id, std::filesystem::path path)
	: m_gen(gen), m_id(id),
	m_generationPath(path / std::to_string(gen)),
	m_inputFilePath(path/std::to_string(gen)/(std::to_string(id)+".input")),
	m_outputFilePath(path / std::to_string(gen) / (std::to_string(id) + ".output")),
	m_input{nullptr},
	m_output{nullptr},
	m_hasInput(false),
	m_hasOutput(false)
{
	std::filesystem::create_directory(m_generationPath);
}

SimulationDataPtr SimulationLog::loadInput() {
	if (!m_input) {
		m_input = importSimulationData(m_inputFilePath.string());
		if (!m_input) {
			return nullptr;
		}
		m_hasInput = true;
	}
	return m_input;
}

SimulationDataPtr SimulationLog::loadOutput() {
	if (!m_output) {
		m_output = importSimulationData(m_outputFilePath.string());
		if (!m_output) {
			return nullptr;
		}
		m_hasOutput = true;
	}
	return m_output;
}

SimulationDataPtr SimulationLog::createInput(SimulationDataPtr sPtr) {
	if (!sPtr) {
		return nullptr;
	}
	spdlog::info("Exporting simulation({}, {}) to {}", m_gen, m_id, m_inputFilePath.string());
	exportSimulationData(sPtr, m_inputFilePath.string());
	m_input = sPtr;
	m_hasInput = true;
	return m_input;
}

SimulationDataPtr SimulationLog::createOutput(SimulationDataPtr sPtr) {
	if (!sPtr) {
		return nullptr;
	}
	exportSimulationData(sPtr, m_outputFilePath.string());
	m_output = sPtr;
	m_hasOutput = true;
	return m_output;
}
