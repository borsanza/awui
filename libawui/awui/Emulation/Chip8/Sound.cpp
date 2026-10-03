/**
 * awui/Emulation/Chip8/Sound.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Sound.h"

#include <awui/Emulation/Common/AudioOutput.h>

using namespace awui::Emulation::Chip8;
using awui::Emulation::Common::AudioOutput;

Sound::Sound() {
	m_playing = false;
	m_phase = 0.0;
	m_pendingSamples = 0.0;
	m_fastForward = false;
}

Sound::~Sound() {
	Stop();
}

void Sound::Play() {
	m_playing = true;
}

void Sound::Stop() {
	m_playing = false;
}

void Sound::EndTick(double seconds) {
	AudioOutput &output = AudioOutput::Instance();
	if (!output.IsPlaying(this))
		return;

	// Muestras de este tick, corrigiendo un poco el ritmo para que la cola no crezca ni se vacíe
	m_pendingSamples += seconds * AudioOutput::Frequency / output.GetRateAdjust();
	int count = (int) m_pendingSamples;
	m_pendingSamples -= count;
	if (count <= 0)
		return;

	if (m_fastForward && output.IsQueueFull())
		return;

	m_samples.resize(count);
	double step = Frequency / AudioOutput::Frequency;
	for (int i = 0; i < count; i++) {
		m_samples[i] = m_playing ? ((m_phase < 0.5) ? Level : -Level) : 0;
		m_phase += step;
		if (m_phase >= 1.0)
			m_phase -= 1.0;
	}

	output.Queue(this, m_samples.data(), count, 1);
}
