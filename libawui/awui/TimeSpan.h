// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUITIMESPAN_H__
#define __AWUITIMESPAN_H__

#include <awui/Object.h>

namespace awui {
	class String;

	class TimeSpan : public Object {
		public:
			const long TicksPerDay = 864000000000;
			const long TicksPerHour = 36000000000;
			const long TicksPerMillisecond = 10000;
			const long TicksPerMinute = 600000000;
			const long TicksPerSecond = 10000000;
		private:
			long long ticks;

			String ConvertDecimals(int value, int decimals);

		public:
			TimeSpan(long long ticks);
			TimeSpan(int hours, int minutes, int seconds);
			TimeSpan(int days, int hours, int minutes, int seconds);
			TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds);
			virtual ~TimeSpan();

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

			virtual String ToString();
	};
}

#endif
