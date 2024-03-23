// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "ChronoLap.h"

using namespace awui;

ChronoLap::ChronoLap() {
	m_running = false;
	m_lastLapDuration = Duration::zero();
	m_totalDuration = Duration::zero();
}

// Inicia o reinicia el ChronoLap
void ChronoLap::Start() {
	m_startTime = Clock::now();
	m_lapTime = m_startTime;
	m_running = true;
}

// Detiene el ChronoLap
void ChronoLap::Lap() {
	if (!m_running) {
		return;
	}

	auto now = Clock::now();
	m_lastLapDuration = now - m_lapTime;
	m_lapTime = now;
	m_totalDuration = now - m_startTime;
}

void ChronoLap::Stop() {
	if (!m_running) {
		return;
	}

	Lap();

	m_running = false;
}

// Retorna la cantidad de segundos transcurridos como float
float ChronoLap::GetLapTime() const {
	return std::chrono::duration_cast<std::chrono::duration<float>>(m_lastLapDuration).count();
}

float ChronoLap::GetTotalTime() const {
	if (m_running) {
		return std::chrono::duration_cast<std::chrono::duration<float>>((Clock::now() - m_startTime)).count();
	}

	return std::chrono::duration_cast<std::chrono::duration<float>>(m_totalDuration).count();
}
