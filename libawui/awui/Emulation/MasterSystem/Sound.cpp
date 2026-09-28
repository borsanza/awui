/*
 * awui/Emulation/MasterSystem/Sound.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Sound.h"

#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Emulation/MasterSystem/SoundSDL.h>
#include <awui/Emulation/MasterSystem/VDP.h>

#include <cmath>

using namespace awui::Emulation::MasterSystem;

#define CLOCK_NTSC 3579545.0
#define CLOCK_PAL 3546893.0

// El chip avanza un paso cada 16 ciclos de reloj
#define CYCLES_PER_TICK 16

namespace {
	// Cada paso de atenuación son 2dB. Con 4 canales al máximo: 4 * 8191 cabe en un int16
	struct VolumeTable {
		int values[16];

		VolumeTable() {
			for (int i = 0; i < 15; i++)
				values[i] = (int) std::lround(8191.0 * std::pow(10.0, -0.1 * i));
			values[15] = 0;
		}
	};

	const VolumeTable volumeTable;
} // namespace

Sound::Sound() {
	m_cpu = NULL;

	// Abre el dispositivo de audio
	SoundSDL::Instance();

	Reset();
}

void Sound::Reset() {
	for (int i = 0; i < 4; i++) {
		m_registers[i] = 0;
		m_volumes[i] = 0xF;
		m_counters[i] = 0;
		m_outputs[i] = 1;
	}

	m_noiseToggle = false;
	m_lfsr = 0x8000;
	m_latchedChannel = 0;
	m_latchedVolume = false;

	m_lastCycle = 0;
	m_pendingCycles = 0;
	m_ticksPerSample = (CLOCK_NTSC / CYCLES_PER_TICK) / SOUNDFREQ;
	m_tickPos = 0;
	m_sampleSum = 0;
	m_sampleTicks = 0;
	m_samples.clear();
}

void Sound::Tick() {
	// Canales de tono: la salida cambia de signo cada vez que el contador llega a 0
	for (int i = 0; i < 3; i++) {
		if (--m_counters[i] <= 0) {
			m_counters[i] = m_registers[i];
			m_outputs[i] = -m_outputs[i];
		}
	}

	// Ruido: el LFSR avanza cada dos recargas del contador
	if (--m_counters[3] <= 0) {
		int rate = m_registers[3] & 0x3;
		m_counters[3] = (rate == 3) ? m_registers[2] : (0x10 << rate);

		m_noiseToggle = !m_noiseToggle;
		if (m_noiseToggle) {
			int feedback;
			if (m_registers[3] & 0x4) {
				// Ruido blanco: bits 0 y 3
				int tapped = m_lfsr & 0x0009;
				feedback = (tapped == 0x0001) || (tapped == 0x0008);
			} else {
				// Ruido periódico
				feedback = m_lfsr & 1;
			}
			m_lfsr = (m_lfsr >> 1) | (feedback << 15);
		}
	}

	m_outputs[3] = (m_lfsr & 1) ? 1 : -1;

	int mix = 0;
	for (int i = 0; i < 4; i++) {
		if (!SoundSDL::IsChannelEnabled(i))
			continue;

		// En el chip de Sega un periodo de 0 o 1 deja la salida fija a +1.
		// Los juegos lo usan para reproducir samples cambiando solo el volumen.
		int output = (i < 3 && m_registers[i] <= 1) ? 1 : m_outputs[i];
		mix += output * volumeTable.values[m_volumes[i]];
	}

	// Promedia todos los pasos del chip que caen en la misma muestra de salida (filtro anti-aliasing sencillo)
	m_sampleSum += mix;
	m_sampleTicks++;
	m_tickPos += 1.0;

	if (m_tickPos >= m_ticksPerSample) {
		m_tickPos -= m_ticksPerSample;
		m_samples.push_back((int16_t) (m_sampleSum / m_sampleTicks));
		m_sampleSum = 0;
		m_sampleTicks = 0;
	}
}

void Sound::Render(int64_t cycle) {
	int64_t delta = cycle - m_lastCycle;
	m_lastCycle = cycle;

	// El contador de ciclos ha ido hacia atrás (reset o carga de estado)
	if (delta <= 0)
		return;

	// Si este emulador no es el que suena, solo se mantienen los registros
	if (!SoundSDL::Instance().IsPlaying(this)) {
		m_pendingCycles = 0;
		return;
	}

	m_pendingCycles += (int) delta;
	while (m_pendingCycles >= CYCLES_PER_TICK) {
		m_pendingCycles -= CYCLES_PER_TICK;
		Tick();
	}
}

void Sound::WriteByte(Motherboard *cpu, uint8_t value) {
	// Primero genera el audio hasta el momento de la escritura
	Render(cpu->GetCycles());

	int channel;
	if (value & 0x80) {
		// Byte LATCH/DATA: %1cctdddd
		m_latchedChannel = (value >> 5) & 0x3;
		m_latchedVolume = (value & 0x10) != 0;
		channel = m_latchedChannel;

		if (m_latchedVolume)
			m_volumes[channel] = value & 0xF;
		else if (channel == 3) {
			m_registers[3] = value & 0x7;
			m_lfsr = 0x8000;
		} else
			m_registers[channel] = (m_registers[channel] & 0x3F0) | (value & 0xF);
	} else {
		// Byte DATA: %0-dddddd, se aplica al último registro seleccionado
		channel = m_latchedChannel;

		if (m_latchedVolume)
			m_volumes[channel] = value & 0xF;
		else if (channel == 3) {
			m_registers[3] = value & 0x7;
			m_lfsr = 0x8000;
		} else
			m_registers[channel] = (m_registers[channel] & 0x00F) | ((value & 0x3F) << 4);
	}
}

void Sound::EndFrame(Motherboard *cpu) {
	SoundSDL &soundSDL = SoundSDL::Instance();

	// El ajuste de ritmo corrige la pequeña diferencia entre el reloj emulado y el de la tarjeta de sonido
	double clock = cpu->GetVDP()->GetNTSC() ? CLOCK_NTSC : CLOCK_PAL;
	m_ticksPerSample = ((clock / CYCLES_PER_TICK) / SOUNDFREQ) * soundSDL.GetRateAdjust();

	Render(cpu->GetCycles());

	if (!m_samples.empty()) {
		soundSDL.Queue(this, m_samples.data(), (int) m_samples.size());
		m_samples.clear();
	}
}
