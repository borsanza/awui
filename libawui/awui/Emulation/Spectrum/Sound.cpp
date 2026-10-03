/*
 * awui/Emulation/Spectrum/Sound.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "Sound.h"

#include <awui/Emulation/Spectrum/Motherboard.h>
#include <awui/Emulation/Common/AudioOutput.h>

#include <algorithm>

using namespace awui::Emulation::Spectrum;

// Amplitud de cada bit (EAR suena bastante más que MIC)
#define EAR_LEVEL 6000
#define MIC_LEVEL 1500

Sound::Sound() {
	m_cpu = NULL;
	m_level = -EAR_LEVEL - MIC_LEVEL;
	m_pendingSamples = 0.0;
	m_dcIn = 0.0f;
	m_dcOut = 0.0f;
	m_reverse = false;
	m_fastForward = false;

	// Abre el dispositivo de audio
	Common::AudioOutput::Instance();
}

void Sound::WriteSound(Motherboard *cpu, int value) {
	int16_t level = (((value >> 4) & 0x01) ? EAR_LEVEL : -EAR_LEVEL) + (((value >> 3) & 0x01) ? MIC_LEVEL : -MIC_LEVEL);

	int16_t last = m_changes.empty() ? m_level : m_changes.back().level;
	if (level == last)
		return;

	double position = std::clamp(cpu->GetFramePosition(), 0.0, 1.0);
	m_changes.push_back({position, level});
}

void Sound::EndFrame(double seconds, bool silent) {
	Common::AudioOutput &output = Common::AudioOutput::Instance();

	// Muestras de este frame, corrigiendo un poco el ritmo para que la cola de SDL no crezca ni se vacíe
	m_pendingSamples += seconds * Common::AudioOutput::Frequency / output.GetRateAdjust();
	int count = (int) m_pendingSamples;
	m_pendingSamples -= count;

	if (count <= 0) {
		return;
	}

	m_samples.resize(count);
	// El volumen lo aplica AudioOutput; en modo rápido se envía silencio
	float gain = silent ? 0.0f : 1.0f;

	// Cada muestra es la media del nivel durante su intervalo: suaviza los flancos de la onda cuadrada
	int16_t current = m_level;
	size_t index = 0;
	for (int i = 0; i < count; i++) {
		double t0 = double(i) / count;
		double t1 = double(i + 1) / count;
		double t = t0;
		double sum = 0.0;

		while ((index < m_changes.size()) && (m_changes[index].position < t1)) {
			double p = std::max(m_changes[index].position, t);
			sum += current * (p - t);
			t = p;
			current = m_changes[index].level;
			index++;
		}

		sum += current * (t1 - t);
		float x = float(sum / (t1 - t0));

		// Paso alto (quita la continua): y = x - x[-1] + R·y[-1]
		float y = x - m_dcIn + 0.995f * m_dcOut;
		m_dcIn = x;
		m_dcOut = y;

		m_samples[i] = (int16_t) std::clamp(y * gain, -32768.0f, 32767.0f);
	}

	m_level = current;
	m_changes.clear();

	// En avance rápido se generan más muestras de las que se pueden tocar: las de este frame se tiran si ya hay
	// bastantes en cola (así no se llena y se vacía de golpe)
	if (m_fastForward && output.IsQueueFull())
		return;

	if (m_reverse)
		std::reverse(m_samples.begin(), m_samples.end());

	output.Queue(this, m_samples.data(), count, 1);
}
