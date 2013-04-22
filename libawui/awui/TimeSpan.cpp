// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "TimeSpan.h"

#include <awui/Int32.h>
#include <awui/Math.h>
#include <awui/String.h>

using namespace awui;

TimeSpan::TimeSpan(long long ticks) {
	this->ticks = ticks;
}

TimeSpan::TimeSpan(int hours, int minutes, int seconds) {
	this->ticks = (hours*TicksPerHour) + (minutes*TicksPerMinute) + (seconds*TicksPerSecond);
}

TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds) {
	this->ticks = (days*TicksPerDay) + (hours*TicksPerHour) + (minutes*TicksPerMinute) + (seconds*TicksPerSecond);
}

TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds) {
	this->ticks = (days*TicksPerDay) + (hours*TicksPerHour) + (minutes*TicksPerMinute) + (seconds*TicksPerSecond) + (milliseconds*TicksPerMillisecond);
}

TimeSpan::~TimeSpan() {
}

long long TimeSpan::GetTicks() const {
	return this->ticks;
}

double TimeSpan::GetTotalMilliseconds() const {
	return (double)this->ticks / TicksPerMillisecond;
}

double TimeSpan::GetTotalSeconds() const {
	return (double)this->ticks / TicksPerSecond;
}

double TimeSpan::GetTotalMinutes() const {
	return (double)this->ticks / TicksPerMinute;
}

double TimeSpan::GetTotalHours() const {
	return (double)this->ticks / TicksPerHour;
}

double TimeSpan::GetTotalDays() const {
	return (double)this->ticks / TicksPerDay;
}

int TimeSpan::GetMilliseconds() const {
	return ((int)Math::Floor(this->GetTotalMilliseconds())) % 1000;
}

int TimeSpan::GetSeconds() const {
	return ((int)Math::Floor(this->GetTotalSeconds())) % 60;
}

int TimeSpan::GetMinutes() const {
	return ((int)Math::Floor(this->GetTotalMinutes())) % 60;
}

int TimeSpan::GetHours() const {
	return ((int)Math::Floor(this->GetTotalHours())) % 24;
}

int TimeSpan::GetDays() const {
	return Math::Floor(this->GetTotalDays());
}

awui::String TimeSpan::ConvertDecimals(int value, int decimals) {
	String output = "";
	for (int i=0; i<decimals; i++) {
		output = Int32(value % 10).ToString() + output;
		value = Math::Floor((float)value / 10);
	}

	return output;
}

awui::String TimeSpan::ToString() {
	String output = "";
	int value;

	value = GetDays();
	if (value > 0) {
		output += Int32(value).ToString() + ".";
	}

	value = GetHours();
	output += Int32(value).ToString() + ":";

	value = GetMinutes();
	output += Int32(value).ToString() + ":";

	value = GetSeconds();
	output += Int32(value).ToString() + ".";

	value = GetTicks() % 10000000;
	output += ConvertDecimals(value, 7);

	return output;
}
