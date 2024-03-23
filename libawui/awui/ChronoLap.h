#pragma once

#include <awui/Object.h>

#include <chrono>

namespace awui {
	class ChronoLap : public Object {
	  private:
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = std::chrono::time_point<Clock>;
		using Duration = Clock::duration;

		TimePoint m_startTime;
		TimePoint m_lapTime;
		Duration m_lastLapDuration;
		Duration m_totalDuration;
		bool m_running;

	  public:
		ChronoLap();

		void Start();
		void Lap();
		void Stop();
		float GetLapTime() const;
		float GetTotalTime() const;
	};
} // namespace awui
