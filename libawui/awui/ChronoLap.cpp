// (c) Copyright 2024 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "ChronoLap.h"

using namespace awui;

ChronoLap::ChronoLap() {
	m_running = false;
	m_lastLapDuration = Duration::zero();
	m_totalDuration = Duration::zero();
}

// Inicia o reinicia el ChronoLap, restableciendo todas las duraciones.
void ChronoLap::Start() {
	m_startTime = Clock::now();
	m_lapTime = m_startTime;
	m_running = true;
	m_lastLapDuration = Duration::zero();
	m_totalDuration = Duration::zero();
}

// Registra la duración de la vuelta actual y actualiza el tiempo total.
void ChronoLap::Lap() {
	if (!m_running) {
		return;
	}

	auto now = Clock::now();
	m_lastLapDuration = now - m_lapTime;
	m_lapTime = now;
	m_totalDuration = now - m_startTime;
}

// Detiene el cronometraje, registrando cualquier tiempo parcial como una vuelta final.
void ChronoLap::Stop() {
	if (!m_running) {
		return;
	}

	Lap();

	m_running = false;
}

// Obtiene el tiempo de la última vuelta en segundos.
float ChronoLap::GetLapTime() const {
	return std::chrono::duration_cast<std::chrono::duration<float>>(m_lastLapDuration).count();
}

// Obtiene el tiempo total transcurrido desde el inicio en segundos.
float ChronoLap::GetTotalTime() const {
	if (m_running) {
		return std::chrono::duration_cast<std::chrono::duration<float>>((Clock::now() - m_startTime)).count();
	}

	return std::chrono::duration_cast<std::chrono::duration<float>>(m_totalDuration).count();
}
