// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "TimeSpan.h"

#include <awui/Convert.h>
#include <awui/Math.h>
#include <awui/String.h>

using namespace awui;

TimeSpan::TimeSpan() {
	this->ticks = 0;
}

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

awui::String TimeSpan::ConvertDecimals(int value, int decimals) const {
	String output = "";
	for (int i=0; i < decimals; i++) {
		output = Convert::ToString(value % 10) + output;
		value = Math::Floor((float)value / 10);
	}

	return output;
}

awui::String TimeSpan::ToString() const {
	String output = "";

	int value = GetDays();
	if (value > 0) {
		output += Convert::ToString(value) + ".";
	}

	output += ConvertDecimals(GetHours(), 2) + ":";
	output += ConvertDecimals(GetMinutes(), 2) + ":";
	output += ConvertDecimals(GetSeconds(), 2) + ".";

	value = GetTicks() % 10000000;
	output += ConvertDecimals(value, 7);

	return output;
}

TimeSpan & TimeSpan::operator= (const TimeSpan & other) {
	this->ticks = other.ticks;

	return *this;
}
