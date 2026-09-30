#pragma once

#include <chrono>
#include <cstdint>

namespace awui::Time {
	class DateTime {
	  private:
		std::chrono::microseconds m_time;

	  public:
		DateTime();
		DateTime(std::chrono::microseconds time);
		~DateTime() = default;

		static DateTime GetNow();
		int64_t GetTicks() const;

		unsigned int GetMillisecond() const;
		unsigned char GetSecond() const;
		unsigned char GetMinute() const;
		unsigned char GetHour() const;
	};
} // namespace awui::Time
