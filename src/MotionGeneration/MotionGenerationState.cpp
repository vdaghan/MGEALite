#include "MotionGeneration/MotionGenerationState.h"

template <typename T>
bool copyIfChanged(T & t, T const & c) {
	bool changed = false;
	if (t != c) [[unlikely]] {
		t = c;
		changed = true;
	}
	return changed;
}

MotionGenerationState::MotionGenerationState() : m_epochProgress{}, m_fitnessStatus{} {}

EpochProgress::EpochProgress() : failed(0), evaluated(0), total(0), changed(false) {}

EAProgress::EAProgress() : currentGeneration(0), numberOfGenerations(0) {}

FitnessStatus::FitnessStatus() : fitnesses(), changed(false) {}

void EpochProgress::updateWith(EpochProgress const & eP) {
	bool epochProgressChanged = false;
	epochProgressChanged |= copyIfChanged(failed, eP.failed);
	epochProgressChanged |= copyIfChanged(evaluated, eP.evaluated);
	epochProgressChanged |= copyIfChanged(total, eP.total);
	changed = epochProgressChanged;
}

void EAProgress::updateWith(EAProgress const & eP) {
	bool eaProgressChanged = false;
	eaProgressChanged |= copyIfChanged(currentGeneration, eP.currentGeneration);
	eaProgressChanged |= copyIfChanged(numberOfGenerations, eP.numberOfGenerations);
	changed = eaProgressChanged;
}

void FitnessStatus::updateWith(FitnessStatus const & fS) {
	bool fitnessStatusChanged = false;
	fitnessStatusChanged |= copyIfChanged(fitnesses, fS.fitnesses);
	changed = fitnessStatusChanged;
}

void MotionGenerationState::updateWith(EpochProgress const & eP) {
	std::lock_guard<std::mutex> lock(changeMutex);
	m_epochProgress.updateWith(eP);
}

void MotionGenerationState::updateWith(EAProgress const & eP) {
	std::lock_guard<std::mutex> lock(changeMutex);
	m_eaProgress.updateWith(eP);
}

void MotionGenerationState::updateWith(FitnessStatus const & fS) {
	std::lock_guard<std::mutex> lock(changeMutex);
	m_fitnessStatus.updateWith(fS);
}

void MotionGenerationState::updateWith(MotionGenerationState const & mGS) {
	updateWith(mGS.m_epochProgress);
	updateWith(mGS.m_eaProgress);
	updateWith(mGS.m_fitnessStatus);
}

StateComponentChanged MotionGenerationState::changed() const {
	std::lock_guard<std::mutex> lock(changeMutex);
	StateComponentChanged retVal(StateComponentChanged_None);
	if (m_epochProgress.changed) {
		retVal = static_cast<StateComponentChanged>(static_cast<std::size_t>(retVal) | static_cast<std::size_t>(StateComponentChanged_EpochProgress));
	}
	if (m_eaProgress.changed) {
		retVal = static_cast<StateComponentChanged>(static_cast<std::size_t>(retVal) | static_cast<std::size_t>(StateComponentChanged_EAProgress));
	}
	if (m_fitnessStatus.changed) {
		retVal = static_cast<StateComponentChanged>(static_cast<std::size_t>(retVal) | static_cast<std::size_t>(StateComponentChanged_FitnessStatus));
	}
	return retVal;
}

EpochProgress const & MotionGenerationState::epochProgress() {
	std::lock_guard<std::mutex> lock(changeMutex);
	m_epochProgress.changed = false;
	return m_epochProgress;
}

EAProgress const & MotionGenerationState::eaProgress() {
	std::lock_guard<std::mutex> lock(changeMutex);
	m_epochProgress.changed = false;
	return m_eaProgress;
}

FitnessStatus const & MotionGenerationState::fitnessStatus() {
	std::lock_guard<std::mutex> lock(changeMutex);
	m_fitnessStatus.changed = false;
	return m_fitnessStatus;
}
