#pragma once

#include <atomic>
#include <stdexcept>
#include <list>
#include <mutex>

class SharedSynchronisation;
class SharedSynchronisationToken {
	public:
		SharedSynchronisationToken(SharedSynchronisationToken && sST);
		SharedSynchronisationToken && createToken();
		bool sync();
		bool check();
	private:
		friend class SharedSynchronisation;
		SharedSynchronisation & sharedSynchronisation;
		std::atomic<bool> & synced;
		std::atomic<bool> & frozen;
		SharedSynchronisationToken(SharedSynchronisation & sS, std::atomic<bool> & s, std::atomic<bool> & f);
};

class SharedSynchronisation {
	public:
		SharedSynchronisation();
		SharedSynchronisationToken && createToken();
		void freeze();
	private:
		friend class SharedSynchronisationToken;
		std::mutex creationMutex;
		std::list<std::atomic<bool>> tokens;
		std::atomic<bool> frozen;
		bool sync();
		bool check();
};
