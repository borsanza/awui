#pragma once

#include <chrono>

namespace awui::Time {
	class ChronoLap {
	  private:
		// Monótono: el reloj del sistema (y high_resolution_clock, que en GCC es el mismo) salta con NTP o al cambiar
		// la hora, y daría deltas negativos o enormes
		using Clock = std::chrono::steady_clock;
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
		float GetLapDuration() const;
		float GetTotalDuration() const;
	};
} // namespace awui::Time
