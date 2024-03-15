// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "DateTime.h"

#include <cstdint>

using namespace awui;

DateTime::DateTime()
	: m_time(0) {
}

DateTime::DateTime(std::chrono::microseconds time)
	: m_time(time) {
}

// Método estático para obtener el momento actual.
DateTime DateTime::GetNow() {
	auto now = std::chrono::system_clock::now().time_since_epoch();
	return DateTime(std::chrono::duration_cast<std::chrono::microseconds>(now));
}

// Devuelve el tiempo interno en "ticks" (1 tick = 100 nanosegundos).
int64_t DateTime::GetTicks() const {
	return m_time.count() * 10;
}

// Devuelve el total de segundos actual.
double DateTime::GetTotalSeconds() {
	auto now = std::chrono::system_clock::now().time_since_epoch();
	return now.count() / 1000000.0;
}

// Devuelve el segundo actual (0-59).
unsigned char DateTime::GetSecond() const {
	return static_cast<unsigned char>((m_time.count() / 1000000) % 60);
}

// Devuelve el minuto actual (0-59).
unsigned char DateTime::GetMinute() const {
	return static_cast<unsigned char>((m_time.count() / 60000000) % 60);
}

// Devuelve la hora actual (0-23).
unsigned char DateTime::GetHour() const {
	return static_cast<unsigned char>((m_time.count() / 3600000000) % 24);
}

// Devuelve el milisegundo actual (0-999).
unsigned int DateTime::GetMillisecond() const {
	return static_cast<unsigned int>((m_time.count() / 1000) % 1000);
}
