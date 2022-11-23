#pragma once

#include <chrono>
#include <list>
#include <thread>

namespace MGEA {
	using TimePoint = decltype(std::chrono::steady_clock::now());
	using TimePair = std::pair<TimePoint, TimePoint>;
	using MillisecondsType = std::chrono::milliseconds;
	struct FPSInfo {
		double limitedFPS;
		double unlimitedFPS;
	};
	class GUIFrameCounter {
		public:
			FPSInfo tickAndWait() {
				lastTimepoints.emplace_front(std::chrono::high_resolution_clock::now());
				lastTimepoints.resize(std::min(lastTimepoints.size(), measurementSamples));
				double timeDiff(timeDifference(lastTimepoints.front(), lastTimepoints.back()));
				double limitedFrameTime(timeDiff / static_cast<double>(lastTimepoints.size()-1));
				double limitedFPS(1000.0 / limitedFrameTime);

				std::size_t totalSleepDuration(0);
				for (auto & sleepDuration : lastSleepDurations) {
					totalSleepDuration += sleepDuration.count();
				}
				double averageSleepDuration(static_cast<double>(totalSleepDuration) / static_cast<double>(lastSleepDurations.size()));
				double unlimitedFrameTime(limitedFrameTime - averageSleepDuration);
				double unlimitedFPS(1000.0 / unlimitedFrameTime);

				double lastTimeDiff(0.0);
				if (lastTimepoints.size() >= 2) [[likely]] {
					lastTimeDiff = timeDifference(*lastTimepoints.begin(), *std::next(lastTimepoints.begin()));
				}
				double sleepDurationDouble((1000.0 / fpsLimit) - lastTimeDiff);
				if (sleepDurationDouble < 0.0) [[unlikely]] {
					sleepDurationDouble = 0.0;
				}
				auto sleepDuration(MillisecondsType(static_cast<std::size_t>(sleepDurationDouble)));
				lastSleepDurations.push_front(sleepDuration);
				lastSleepDurations.resize(lastTimepoints.size());
				if (sleepDuration.count() > 0) [[likely]] {
					std::this_thread::sleep_for(sleepDuration);
				}

				return FPSInfo{
					.limitedFPS = limitedFPS,
					.unlimitedFPS = unlimitedFPS
				};
			};
			void setFPSLimit(double limit) { fpsLimit = limit; };
			void setMeasurementInterval(double intervalInMilliseconds) {
				measurementSamples = static_cast<std::size_t>(intervalInMilliseconds / (1000.0 / fpsLimit));
			};
		private:
			std::list<TimePoint> lastTimepoints;
			std::list<MillisecondsType> lastSleepDurations;
			double timeDifference(TimePoint tp1, TimePoint tp2) {
				auto diff(tp1-tp2);
				MillisecondsType diffInMS(std::chrono::duration_cast<MillisecondsType>(diff));
				return static_cast<double>(diffInMS.count());
			};
			double fpsLimit;
			std::size_t measurementSamples;
	};
};
