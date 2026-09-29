/*
 * awui/Emulation/Spectrum/Sound.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "Sound.h"

#include <awui/Emulation/Common/AudioSettings.h>
#include <awui/Emulation/Spectrum/Motherboard.h>
#include <awui/Emulation/Spectrum/SoundSDL.h>

#include <algorithm>

using namespace awui::Emulation::Spectrum;

// Amplitud de cada bit (EAR suena bastante más que MIC)
#define EAR_LEVEL 6000
#define MIC_LEVEL 1500

Sound::Sound() {
	this->_cpu = NULL;
	this->_level = -EAR_LEVEL - MIC_LEVEL;
	this->_pendingSamples = 0.0;
	this->_dcIn = 0.0f;
	this->_dcOut = 0.0f;

	// Abre el dispositivo de audio
	SoundSDL::Instance();
}

void Sound::WriteSound(Motherboard *cpu, int value) {
	int16_t level = (((value >> 4) & 0x01) ? EAR_LEVEL : -EAR_LEVEL) + (((value >> 3) & 0x01) ? MIC_LEVEL : -MIC_LEVEL);

	int16_t last = this->_changes.empty() ? this->_level : this->_changes.back().level;
	if (level == last)
		return;

	double position = std::clamp(cpu->GetFramePosition(), 0.0, 1.0);
	this->_changes.push_back({position, level});
}

void Sound::EndFrame(double seconds, bool silent) {
	SoundSDL *soundSDL = SoundSDL::Instance();

	// Muestras de este frame, corrigiendo un poco el ritmo para que la cola de SDL no crezca ni se vacíe
	this->_pendingSamples += seconds * SOUNDFREQ * soundSDL->GetRateAdjust();
	int count = (int) this->_pendingSamples;
	this->_pendingSamples -= count;

	if (count <= 0) {
		return;
	}

	this->_samples.resize(count);
	int gain = silent ? 0 : Common::AudioSettings::GetGain();

	// Cada muestra es la media del nivel durante su intervalo: suaviza los flancos de la onda cuadrada
	int16_t current = this->_level;
	size_t index = 0;
	for (int i = 0; i < count; i++) {
		double t0 = double(i) / count;
		double t1 = double(i + 1) / count;
		double t = t0;
		double sum = 0.0;

		while ((index < this->_changes.size()) && (this->_changes[index].position < t1)) {
			double p = std::max(this->_changes[index].position, t);
			sum += current * (p - t);
			t = p;
			current = this->_changes[index].level;
			index++;
		}

		sum += current * (t1 - t);
		float x = float(sum / (t1 - t0));

		// Paso alto (quita la continua): y = x - x[-1] + R·y[-1]
		float y = x - this->_dcIn + 0.995f * this->_dcOut;
		this->_dcIn = x;
		this->_dcOut = y;

		this->_samples[i] = (int16_t) std::clamp((y * gain) / 100.0f, -32768.0f, 32767.0f);
	}

	this->_level = current;
	this->_changes.clear();

	soundSDL->Queue(this, this->_samples.data(), count);
}
