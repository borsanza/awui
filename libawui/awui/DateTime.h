#pragma once

#include <awui/Object.h>

namespace awui {
	class DateTime : public Object {
		private:
			long long _time;

		public:
			DateTime();
			DateTime(long long time);
			virtual ~DateTime();

			static DateTime GetNow();
			long long GetTicks();
			static double GetTotalSeconds();

			unsigned int GetMillisecond();
			unsigned char GetSecond();
			unsigned char GetMinute();
			unsigned char GetHour();
	};
}
