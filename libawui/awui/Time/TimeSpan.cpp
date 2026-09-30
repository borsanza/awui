// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "TimeSpan.h"

#include <awui/Convert.h>
#include <awui/String.h>

using namespace awui;
using namespace awui::Time;

TimeSpan::TimeSpan() {
	m_ticks = 0;
}

TimeSpan::TimeSpan(int64_t ticks) {
	m_ticks = ticks;
}

TimeSpan::TimeSpan(int hours, int minutes, int seconds) {
	m_ticks = (hours * TicksPerHour) + (minutes * TicksPerMinute) + (seconds * TicksPerSecond);
}

TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds) {
	m_ticks = (days * TicksPerDay) + (hours * TicksPerHour) + (minutes * TicksPerMinute) + (seconds * TicksPerSecond);
}

TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds) {
	m_ticks = (days * TicksPerDay) + (hours * TicksPerHour) + (minutes * TicksPerMinute) + (seconds * TicksPerSecond) + (milliseconds * TicksPerMillisecond);
}

int64_t TimeSpan::GetTicks() const {
	return m_ticks;
}

double TimeSpan::GetTotalMilliseconds() const {
	return (double) m_ticks / TicksPerMillisecond;
}

double TimeSpan::GetTotalSeconds() const {
	return (double) m_ticks / TicksPerSecond;
}

double TimeSpan::GetTotalMinutes() const {
	return (double) m_ticks / TicksPerMinute;
}

double TimeSpan::GetTotalHours() const {
	return (double) m_ticks / TicksPerHour;
}

double TimeSpan::GetTotalDays() const {
	return (double) m_ticks / TicksPerDay;
}

// Con enteros: pasando por double e int, los milisegundos se desbordaban a partir de 24 días
int TimeSpan::GetMilliseconds() const {
	return (int) ((m_ticks / TicksPerMillisecond) % 1000);
}

int TimeSpan::GetSeconds() const {
	return (int) ((m_ticks / TicksPerSecond) % 60);
}

int TimeSpan::GetMinutes() const {
	return (int) ((m_ticks / TicksPerMinute) % 60);
}

int TimeSpan::GetHours() const {
	return (int) ((m_ticks / TicksPerHour) % 24);
}

int TimeSpan::GetDays() const {
	return (int) (m_ticks / TicksPerDay);
}

awui::String TimeSpan::ConvertDecimals(int value, int decimals) const {
	String output = "";
	for (int i = 0; i < decimals; i++) {
		output = Convert::ToString(value % 10) + output;
		value /= 10;
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

	value = (int) (GetTicks() % TicksPerSecond);
	output += ConvertDecimals(value, 7);

	return output;
}
