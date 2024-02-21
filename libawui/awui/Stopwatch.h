#pragma once

#include <awui/Object.h>

#include <chrono>

namespace awui {
	class Stopwatch : public Object {
		private:
			std::chrono::steady_clock::time_point m_start_time;
			std::chrono::steady_clock::time_point m_end_time;
			bool m_running = false;

		public:
			Stopwatch() = default;

			void StartNew();
			void Stop();
			float GetDeltaTime() const;
	};
}
