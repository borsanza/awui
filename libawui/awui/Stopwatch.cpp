// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Stopwatch.h"

using namespace awui;

// Inicia o reinicia el Stopwatch
void Stopwatch::StartNew() {
	m_start_time = std::chrono::steady_clock::now();
	m_running = true;
}

// Detiene el Stopwatch
void Stopwatch::Stop() {
	if (m_running) {
		m_end_time = std::chrono::steady_clock::now();
		m_running = false;
	}
}

// Retorna la cantidad de milisegundos transcurridos
float Stopwatch::GetDeltaSeconds() const {
	auto end = m_running ? std::chrono::steady_clock::now() : m_end_time;
	return std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_start_time).count() / 1000000000.0f;
}
