#pragma once

#include <awui/Object.h>

#include <chrono>

namespace awui {
	class Stopwatch : public Object {
	  private:
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = std::chrono::time_point<Clock>;
		using Duration = Clock::duration;

		TimePoint m_start_time;
		Duration m_elapsed;
		bool m_running;

	  public:
		Stopwatch();

		void StartNew();
		void Stop();
		float GetDeltaSeconds() const;
	};
} // namespace awui
