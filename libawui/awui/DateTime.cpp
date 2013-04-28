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
	timeval tv;
	timezone tz;
	gettimeofday(&tv, &tz);

	return DateTime((tv.tv_sec * 1000000) + tv.tv_usec);
}

long long DateTime::GetTicks() {
	return this->_time * 10;
}
