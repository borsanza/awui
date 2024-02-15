// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "DateTime.h"

#include <sys/time.h>

using namespace awui;

DateTime::DateTime() {
	m_time = 0;
}

DateTime::DateTime(long long time) {
	m_time = time;
}

DateTime::~DateTime() {
}

DateTime DateTime::GetNow() {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);

	return DateTime((tv.tv_sec * 1000000) + tv.tv_usec);
}

long long DateTime::GetTicks() {
	return m_time * 10;
}

double DateTime::GetTotalSeconds() {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	return tv.tv_sec + (tv.tv_usec / 1000000.0);
}

unsigned char DateTime::GetSecond() {
	return (m_time / 1000000) % 60;
}

unsigned char DateTime::GetMinute() {
	return (m_time / 60000000) % 60;
}

unsigned char DateTime::GetHour() {
	return (m_time / 3600000000) % 24;
}

unsigned int DateTime::GetMillisecond() {
	return m_time % 1000000;
}
