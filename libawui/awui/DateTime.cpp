// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "DateTime.h"

#include <sys/time.h>

using namespace awui;

DateTime::DateTime() {
	this->_time = 0;
}

DateTime::DateTime(long long time) {
	this->_time = time;
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
	return this->_time * 10;
}

double DateTime::GetTotalSeconds() {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	return tv.tv_sec + (tv.tv_usec / 1000000.0);
}

unsigned char DateTime::GetSecond() {
	return (this->_time / 1000000) % 60;
}

unsigned char DateTime::GetMinute() {
	return (this->_time / 60000000) % 60;
}

unsigned char DateTime::GetHour() {
	return (this->_time / 3600000000) % 24;
}

unsigned int DateTime::GetMillisecond() {
	return this->_time % 1000000;
}
