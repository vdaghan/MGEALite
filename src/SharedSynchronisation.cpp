#include "SharedSynchronisation.h"

SharedSynchronisation::SharedSynchronisation()
: nextTokenId(0) {
}

SharedSynchronisationToken SharedSynchronisation::createToken() {
	//std::lock_guard<std::mutex> lock(creationMutex);
	//if (finalised.load()) {
	//    throw std::logic_error("SharedSynchronisation should not be asked for a new Token once it was frozen.");
	//}
	auto tokenId(nextTokenId++);
	return std::move(SharedSynchronisationToken(*this, tokenId));
}

void SharedSynchronisation::registerEvent(SyncEvent sE, bool flagValue) {
	if (events.contains(sE)) {
		return;
	}
	events.emplace(sE);
	flags.emplace(std::make_pair(sE, flagValue));
	finalised.emplace(std::make_pair(sE, false));
}

void SharedSynchronisation::finalise(SyncEvent sE) {
	if (!events.contains(sE)) {
		std::cout << "finalise(" << sE << "): event not registered." << std::endl;
		return;
	}
	finalised[sE].store(true);
}

void SharedSynchronisation::finaliseAll() {
	for (auto & event : events) {
		finalised[event].store(true);
	}
}

bool SharedSynchronisation::addCallback(SyncEvent sE, CallbackType cT, std::function<void(void)> cB) {
	if (!isRegisteredAndFinalised(sE)) {
		return false;
	}
	callbacks[sE].emplace_back(std::make_pair(cT, cB));
	return true;
}

bool SharedSynchronisation::setAndCall(SyncEvent sE, FlagSetType fST) {
	if (!isRegisteredAndFinalised(sE)) {
		std::cout << "sync(" << sE << "): event not registered or finalised." << std::endl;
		return false;
	}
	setFlag(sE, fST);
	callCallbacks(sE);
	return true;
}

bool SharedSynchronisation::get(SyncEvent sE) {
	return getFlag(sE);
}

bool SharedSynchronisation::isRegistered(SyncEvent sE) {
	return events.contains(sE);
}

bool SharedSynchronisation::isRegisteredAndFinalised(SyncEvent sE) {
	return isRegistered(sE) and finalised.at(sE).load();
}

bool SharedSynchronisation::getFlag(SyncEvent sE) {
	if (!isRegisteredAndFinalised(sE)) {
		return false;
	}
	return flags.at(sE).load();
}

void SharedSynchronisation::setFlag(SyncEvent sE, FlagSetType fST) {
	if (!isRegisteredAndFinalised(sE)) {
		return;
	}
	if (FlagSetType::True == fST) {
		flags.at(sE).store(true);
	} else if (FlagSetType::False == fST) {
		flags.at(sE).store(false);
	} else if (FlagSetType::Toggle == fST) {
		flags.at(sE).store(!flags.at(sE).load());
	}
}

void SharedSynchronisation::callCallbacks(SyncEvent sE) {
	if (!isRegisteredAndFinalised(sE) or !callbacks.contains(sE)) {
		return;
	}
	for (auto & callbackPair : callbacks.at(sE)) {
		auto & callbackType(callbackPair.first);
		auto & callbackFunction(callbackPair.second);
		if (CallbackType::OnToggle == callbackType) {
			callbackFunction();
			continue;
		}
		auto flagValue(getFlag(sE));
		if (flagValue and CallbackType::OnTrue == callbackType) {
			callbackFunction();
		}
		if (!flagValue and CallbackType::OnFalse == callbackType) {
			callbackFunction();
		}
	}
}

SharedSynchronisationToken::SharedSynchronisationToken(SharedSynchronisationToken && sST) noexcept
: sharedSynchronisation(sST.sharedSynchronisation)
, tokenId(sST.tokenId)
{}

SharedSynchronisationToken SharedSynchronisationToken::createToken() {
	return std::move(sharedSynchronisation.createToken());
}

bool SharedSynchronisationToken::addCallback(SyncEvent sE, CallbackType cT, std::function<void(void)> cB) {
	return sharedSynchronisation.addCallback(sE, cT, cB);
}

bool SharedSynchronisationToken::setAndCall(SyncEvent sE, FlagSetType fST) {
	return sharedSynchronisation.setAndCall(sE, fST);
}

bool SharedSynchronisationToken::get(SyncEvent sE) {
	return sharedSynchronisation.get(sE);
}

SharedSynchronisationToken::SharedSynchronisationToken(SharedSynchronisation & sS, std::size_t tId)
: sharedSynchronisation(sS)
, tokenId(tId)
{}
