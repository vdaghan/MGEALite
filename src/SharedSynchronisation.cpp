#include "SharedSynchronisation.h"

SharedSynchronisationToken::SharedSynchronisationToken(SharedSynchronisationToken && sST)
	: sharedSynchronisation(sST.sharedSynchronisation)
	, synced(sST.synced)
	, frozen(sST.frozen)
{}

SharedSynchronisationToken && SharedSynchronisationToken::createToken() {
	return sharedSynchronisation.createToken();
}

bool SharedSynchronisationToken::sync() {
	if (!frozen.load()) {
		return false;
	}
	sharedSynchronisation.sync();
	return true;
};

bool SharedSynchronisationToken::check() {
	if (!frozen.load()) {
		return false;
	}
	return sharedSynchronisation.check();
};

SharedSynchronisationToken::SharedSynchronisationToken(SharedSynchronisation & sS, std::atomic<bool> & s, std::atomic<bool> & f)
	: sharedSynchronisation(sS)
	, synced(s)
	, frozen(f)
{}

SharedSynchronisation::SharedSynchronisation() : frozen(false) {};

SharedSynchronisationToken && SharedSynchronisation::createToken() {
	std::lock_guard<std::mutex> lock(creationMutex);
	if (frozen.load()) {
		throw std::logic_error("SharedSynchronisation should not be asked for a new Token once it was frozen.");
	}
	tokens.emplace_back();
	SharedSynchronisationToken newToken(*this, tokens.back(), frozen);
	return std::move(newToken);
}

void SharedSynchronisation::freeze() {
frozen.store(true);
}

bool SharedSynchronisation::sync() {
	if (!frozen.load()) {
		return false;
	}
	for (auto & token : tokens) {
		token.store(true);
	}
	return true;
}

bool SharedSynchronisation::check() {
	bool retVal(true);
	for (auto & token : tokens) {
		if (!token.load()) {
			retVal = false;
			break;
		}
	}
	return retVal;
}
