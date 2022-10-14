#pragma once

#include <atomic>
#include <functional>
#include <list>
#include <map>
#include <unordered_map>
#include <mutex>
#include <unordered_set>
#include <stdexcept>
#include <string>

#include <iostream>

using SyncEvent = std::string;
enum class FlagSetType { True, False, Toggle };
enum class CallbackType { OnTrue, OnFalse, OnToggle };
class SharedSynchronisationToken;
class SharedSynchronisation {
	public:
		SharedSynchronisation();
		SharedSynchronisationToken createToken();
		void registerEvent(SyncEvent sE, bool flagValue);
		void finalise(SyncEvent sE);
		void finaliseAll();
		bool addCallback(SyncEvent sE, CallbackType cT, std::function<void(void)> cB);
		bool setAndCall(SyncEvent sE, FlagSetType fST);
		bool get(SyncEvent sE);
	private:
		friend class SharedSynchronisationToken;

		std::mutex creationMutex;
		std::size_t nextTokenId;
		std::unordered_set<SyncEvent> events;
		std::unordered_map<SyncEvent, std::atomic<bool>> flags;
		std::unordered_map<SyncEvent, std::list<std::pair<CallbackType, std::function<void(void)>>>> callbacks;
		std::unordered_map<SyncEvent, std::atomic<bool>> finalised;
		bool isRegistered(SyncEvent sE);
		bool isRegisteredAndFinalised(SyncEvent sE);
		bool getFlag(SyncEvent sE);
		void setFlag(SyncEvent sE, FlagSetType fST);
		void callCallbacks(SyncEvent sE);
};

class SharedSynchronisationToken {
	public:
		SharedSynchronisationToken(SharedSynchronisationToken && sST) noexcept;
		SharedSynchronisationToken createToken();
		bool addCallback(SyncEvent sE, CallbackType cT, std::function<void(void)> cB);
		bool setAndCall(SyncEvent sE, FlagSetType fST);
		bool get(SyncEvent sE);
	private:
		friend class SharedSynchronisation;
		SharedSynchronisation & sharedSynchronisation;
		std::size_t tokenId;
		SharedSynchronisationToken(SharedSynchronisation & sS, std::size_t tId);
};

//int main() {
//	SharedSynchronisation sS;
//	sS.registerEvent("callback1");
//	sS.registerEvent("callback2");
//	auto sST1(sS.createToken());
//	auto callback1Lambda = [&]() {std::cout << "Hello from callback1" << std::endl; };
//	sST1.addCallback("callback1", CallbackType::OnToggle, callback1Lambda);
//	auto sST2(sS.createToken());
//	auto callback2Lambda = [&]() {std::cout << "Hello from callback2" << std::endl; };
//	sST2.addCallback("callback1", CallbackType::OnToggle, callback1Lambda);
//	sST1.addCallback("callback2", CallbackType::OnToggle, callback2Lambda);
//	sST2.addCallback("callback2", CallbackType::OnToggle, callback2Lambda);
//
//	sS.finalise("callback1");
//	sST1.setAndCall("callback1", FlagSetType::True);
//	sST1.setAndCall("callback2", FlagSetType::True);
//	sS.finalise("callback2");
//	sST1.setAndCall("callback2", FlagSetType::True);
//	return 0;
//}