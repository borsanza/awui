// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Stopwatch.h"

using namespace awui;

Stopwatch::Stopwatch() {
	m_running = false;
	m_elapsed = Duration::zero();
}

// Inicia o reinicia el Stopwatch
void Stopwatch::StartNew() {
	m_elapsed = Duration::zero();
	m_start_time = Clock::now();
	m_running = true;
}

// Detiene el Stopwatch
void Stopwatch::Stop() {
	if (m_running) {
		m_elapsed += Clock::now() - m_start_time;
		m_running = false;
	}
}

// Retorna la cantidad de segundos transcurridos como float
float Stopwatch::GetDeltaSeconds() const {
	Duration elapsed = m_running ? m_elapsed + Clock::now() - m_start_time : m_elapsed;
	return std::chrono::duration_cast<std::chrono::duration<float>>(elapsed).count();
}
