// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIDATETIME_H__
#define __AWUIDATETIME_H__

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

			unsigned int GetMillisecond();
			unsigned char GetSecond();
			unsigned char GetMinute();
			unsigned char GetHour();
	};
}

#endif
