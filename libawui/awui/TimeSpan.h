#pragma once

#include <awui/Object.h>

namespace awui {
	class String;

	class TimeSpan : public Object {
		public:
			const long long TicksPerDay = 864000000000;
			const long long TicksPerHour = 36000000000;
			const long TicksPerMillisecond = 10000;
			const long TicksPerMinute = 600000000;
			const long TicksPerSecond = 10000000;
		private:
			long long ticks;

			String ConvertDecimals(int value, int decimals) const;

		public:
			TimeSpan();
			TimeSpan(long long ticks);
			TimeSpan(int hours, int minutes, int seconds);
			TimeSpan(int days, int hours, int minutes, int seconds);
			TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds);
			virtual ~TimeSpan() = default;

			long long GetTicks() const;
			double GetTotalMilliseconds() const;
			double GetTotalSeconds() const;
			double GetTotalMinutes() const;
			double GetTotalHours() const;
			double GetTotalDays() const;

			int GetMilliseconds() const;
			int GetSeconds() const;
			int GetMinutes() const;
			int GetHours() const;
			int GetDays() const;

			virtual String ToString() const override;

			TimeSpan & operator= (const TimeSpan & other);
	};
}
