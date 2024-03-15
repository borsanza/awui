#pragma once

#include <awui/Object.h>

#include <chrono>

namespace awui {
	class DateTime : public Object {
	  private:
		std::chrono::microseconds m_time;

	  public:
		DateTime();
		DateTime(std::chrono::microseconds time);
		virtual ~DateTime() = default;

		static DateTime GetNow();
		long long GetTicks() const;
		static double GetTotalSeconds();

		unsigned int GetMillisecond() const;
		unsigned char GetSecond() const;
		unsigned char GetMinute() const;
		unsigned char GetHour() const;
	};
} // namespace awui
