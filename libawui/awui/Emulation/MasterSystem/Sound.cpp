/*
 * awui/Emulation/MasterSystem/Sound.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Sound.h"

#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Emulation/MasterSystem/SoundSDL.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <awui/Emulation/MasterSystem/emu2413/emu2413.h>

#include <algorithm>
#include <cmath>
#include <cstring>

using namespace awui::Emulation::MasterSystem;

#define CLOCK_NTSC 3579545.0
#define CLOCK_PAL 3546893.0

// El chip avanza un paso cada 16 ciclos de reloj
#define CYCLES_PER_TICK 16

// Silencio tras un salto de rebobinado y duración del fundido de entrada
#define TIME_JUMP_MUTE_SECONDS 0.12
#define FADE_IN_SAMPLES (SOUNDFREQ / 100)

// Mezcla del FM con el PSG: emu2413 da una salida más baja que la de los 4 canales del PSG.
// Con 3.5 la música de Out Run tiene el mismo volumen medio con FM que con PSG
#define FM_GAIN_NUM 7
#define FM_GAIN_DEN 2

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

	// El YM2413 va con el mismo reloj que la CPU; emu2413 convierte su salida (reloj / 72) a SOUNDFREQ
	m_opll = OPLL_new((uint32_t) CLOCK_NTSC, SOUNDFREQ);
	m_muteSamples = 0;
	m_fadeSamples = FADE_IN_SAMPLES;

	Reset();
}

Sound::~Sound() {
	OPLL_delete(m_opll);
}

void Sound::Reset() {
	for (int i = 0; i < 4; i++) {
		m_saveData._registers[i] = 0;
		m_saveData._volumes[i] = 0xF;
		m_saveData._counters[i] = 0;
		m_saveData._outputs[i] = 1;
	}

	m_saveData._noiseToggle = false;
	m_saveData._lfsr = 0x8000;
	m_saveData._latchedChannel = 0;
	m_saveData._latchedVolume = false;

	m_lastCycle = 0;
	m_saveData._pendingCycles = 0;
	m_ticksPerSample = (CLOCK_NTSC / CYCLES_PER_TICK) / SOUNDFREQ;
	m_tickPos = 0;
	m_sampleSumLeft = 0;
	m_sampleSumRight = 0;
	m_saveData._stereo = 0xFF;
	m_sampleTicks = 0;
	m_samples.clear();

	memset(m_saveData._fmRegisters, 0, sizeof(m_saveData._fmRegisters));
	m_saveData._fmAddress = 0;
	m_saveData._fmControl = 0;
	OPLL_reset(m_opll);
}

void Sound::Tick() {
	// Canales de tono: la salida cambia de signo cada vez que el contador llega a 0
	for (int i = 0; i < 3; i++) {
		if (--m_saveData._counters[i] <= 0) {
			m_saveData._counters[i] = m_saveData._registers[i];
			m_saveData._outputs[i] = -m_saveData._outputs[i];
		}
	}

	// Ruido: el LFSR avanza cada dos recargas del contador
	if (--m_saveData._counters[3] <= 0) {
		int rate = m_saveData._registers[3] & 0x3;
		m_saveData._counters[3] = (rate == 3) ? m_saveData._registers[2] : (0x10 << rate);

		m_saveData._noiseToggle = !m_saveData._noiseToggle;
		if (m_saveData._noiseToggle) {
			int feedback;
			if (m_saveData._registers[3] & 0x4) {
				// Ruido blanco: bits 0 y 3
				int tapped = m_saveData._lfsr & 0x0009;
				feedback = (tapped == 0x0001) || (tapped == 0x0008);
			} else {
				// Ruido periódico
				feedback = m_saveData._lfsr & 1;
			}
			m_saveData._lfsr = (m_saveData._lfsr >> 1) | (feedback << 15);
		}
	}

	m_saveData._outputs[3] = (m_saveData._lfsr & 1) ? 1 : -1;

	int left = 0;
	int right = 0;
	for (int i = 0; i < 4; i++) {
		if (!SoundSDL::IsChannelEnabled(i))
			continue;

		// En el chip de Sega un periodo de 0 o 1 deja la salida fija a +1.
		// Los juegos lo usan para reproducir samples cambiando solo el volumen.
		int output = (i < 3 && m_saveData._registers[i] <= 1) ? 1 : m_saveData._outputs[i];
		int value = output * volumeTable.values[m_saveData._volumes[i]];

		// En Master System m_saveData._stereo es siempre 0xFF: los dos lados suenan igual
		if (m_saveData._stereo & (0x10 << i))
			left += value;
		if (m_saveData._stereo & (0x01 << i))
			right += value;
	}

	// Promedia todos los pasos del chip que caen en la misma muestra de salida (filtro anti-aliasing sencillo)
	m_sampleSumLeft += left;
	m_sampleSumRight += right;
	m_sampleTicks++;
	m_tickPos += 1.0;

	if (m_tickPos >= m_ticksPerSample) {
		m_tickPos -= m_ticksPerSample;

		int psgLeft = m_sampleSumLeft / m_sampleTicks;
		int psgRight = m_sampleSumRight / m_sampleTicks;

		// Puerto 0xF2: qué chips suenan. Sin FM (ajuste o Game Gear) solo el PSG
		int mode = m_fmEnabled ? (m_saveData._fmControl & 0x03) : 0;
		int fm = 0;
		if (m_fmEnabled) {
			// Se calcula siempre para que el chip no se quede atrás aunque esté callado
			int value = OPLL_calc(m_opll);
			if (mode & 0x01)
				fm = (value * FM_GAIN_NUM) / FM_GAIN_DEN;
		}

		if (mode == 1 || mode == 2) {
			psgLeft = 0;
			psgRight = 0;
		}

		int left = psgLeft + fm;
		int right = psgRight + fm;
		if (m_muteSamples > 0) {
			m_muteSamples--;
			left = 0;
			right = 0;
		} else if (m_fadeSamples < FADE_IN_SAMPLES) {
			m_fadeSamples++;
			left = (left * m_fadeSamples) / FADE_IN_SAMPLES;
			right = (right * m_fadeSamples) / FADE_IN_SAMPLES;
		}

		m_samples.push_back((int16_t) std::clamp(left, -32768, 32767));
		m_samples.push_back((int16_t) std::clamp(right, -32768, 32767));
		m_sampleSumLeft = 0;
		m_sampleSumRight = 0;
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
		m_saveData._pendingCycles = 0;
		return;
	}

	m_saveData._pendingCycles += (int) delta;
	while (m_saveData._pendingCycles >= CYCLES_PER_TICK) {
		m_saveData._pendingCycles -= CYCLES_PER_TICK;
		Tick();
	}
}

void Sound::WriteByte(Motherboard *cpu, uint8_t value) {
	// Primero genera el audio hasta el momento de la escritura
	Render(cpu->GetCycles());

	int channel;
	if (value & 0x80) {
		// Byte LATCH/DATA: %1cctdddd
		m_saveData._latchedChannel = (value >> 5) & 0x3;
		m_saveData._latchedVolume = (value & 0x10) != 0;
		channel = m_saveData._latchedChannel;

		if (m_saveData._latchedVolume)
			m_saveData._volumes[channel] = value & 0xF;
		else if (channel == 3) {
			m_saveData._registers[3] = value & 0x7;
			m_saveData._lfsr = 0x8000;
		} else
			m_saveData._registers[channel] = (m_saveData._registers[channel] & 0x3F0) | (value & 0xF);
	} else {
		// Byte DATA: %0-dddddd, se aplica al último registro seleccionado
		channel = m_saveData._latchedChannel;

		if (m_saveData._latchedVolume)
			m_saveData._volumes[channel] = value & 0xF;
		else if (channel == 3) {
			m_saveData._registers[3] = value & 0x7;
			m_saveData._lfsr = 0x8000;
		} else
			m_saveData._registers[channel] = (m_saveData._registers[channel] & 0x00F) | ((value & 0x3F) << 4);
	}
}

void Sound::WriteFMAddress(uint8_t value) {
	m_saveData._fmAddress = value & 0x3F;
}

void Sound::WriteFMData(Motherboard *cpu, uint8_t value) {
	// Primero genera el audio hasta el momento de la escritura
	Render(cpu->GetCycles());
	m_saveData._fmRegisters[m_saveData._fmAddress] = value;
	OPLL_writeReg(m_opll, m_saveData._fmAddress, value);
}

void Sound::WriteFMControl(Motherboard *cpu, uint8_t value) {
	Render(cpu->GetCycles());
	// Se guardan 3 bits: la detección los comprueba todos (el modo solo usa los 2 de abajo)
	m_saveData._fmControl = value & 0x07;
}

void Sound::WriteStereo(Motherboard *cpu, uint8_t value) {
	// Genera el audio hasta el momento de la escritura antes de cambiar el reparto
	Render(cpu->GetCycles());
	m_saveData._stereo = value;
}

int Sound::GetSaveSize() {
	return sizeof(Sound::saveData);
}

void Sound::SaveState(uint8_t *data) {
	memcpy(data, &m_saveData, sizeof(Sound::saveData));
}

// cycle: contador de ciclos de la CPU ya restaurada. El audio se resincroniza ahí
// sin generar el salto de tiempo (hacia atrás o hacia delante) del estado cargado.
void Sound::LoadState(uint8_t *data, int64_t cycle) {
	memcpy(&m_saveData, data, sizeof(Sound::saveData));

	// El estado interno de emu2413 no se guarda: se rehace el chip con sus registros (primero instrumento
	// y frecuencias, al final los de nota para que las notas activas vuelvan a sonar)
	OPLL_reset(m_opll);
	for (int reg = 0; reg < 0x40; reg++) {
		if ((reg & 0xF0) != 0x20)
			OPLL_writeReg(m_opll, reg, m_saveData._fmRegisters[reg]);
	}
	for (int reg = 0x20; reg < 0x30; reg++)
		OPLL_writeReg(m_opll, reg, m_saveData._fmRegisters[reg]);

	m_lastCycle = cycle;
	m_tickPos = 0;
	m_sampleSumLeft = 0;
	m_sampleSumRight = 0;
	m_sampleTicks = 0;
	m_samples.clear();
}

void Sound::OnTimeJump() {
	m_samples.clear();
	m_muteSamples = (int) (TIME_JUMP_MUTE_SECONDS * SOUNDFREQ);
	m_fadeSamples = 0;
	SoundSDL::Instance().ClearQueue(this);
}

void Sound::EndFrame(Motherboard *cpu) {
	SoundSDL &soundSDL = SoundSDL::Instance();

	// El ajuste de ritmo corrige la pequeña diferencia entre el reloj emulado y el de la tarjeta de sonido
	double clock = cpu->GetVDP()->GetNTSC() ? CLOCK_NTSC : CLOCK_PAL;
	m_ticksPerSample = ((clock / CYCLES_PER_TICK) / SOUNDFREQ) * soundSDL.GetRateAdjust();

	Render(cpu->GetCycles());

	if (!m_samples.empty()) {
		soundSDL.Queue(this, m_samples.data(), (int) (m_samples.size() / 2));
		m_samples.clear();
	}
}
