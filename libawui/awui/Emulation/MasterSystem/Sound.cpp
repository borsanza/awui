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

// Duración del fundido de entrada al cambiar de sentido (rebobinado)
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
	m_reverse = false;
	m_fadeSamples = FADE_IN_SAMPLES;

	Reset();
}

Sound::~Sound() {
	OPLL_delete(m_opll);
}

void Sound::Reset() {
	for (int i = 0; i < 4; i++) {
		m_saveData.registers[i] = 0;
		m_saveData.volumes[i] = 0xF;
		m_saveData.counters[i] = 0;
		m_saveData.outputs[i] = 1;
	}

	m_saveData.noiseToggle = false;
	m_saveData.lfsr = 0x8000;
	m_saveData.latchedChannel = 0;
	m_saveData.latchedVolume = false;

	m_lastCycle = 0;
	m_saveData.pendingCycles = 0;
	m_ticksPerSample = (CLOCK_NTSC / CYCLES_PER_TICK) / SOUNDFREQ;
	m_tickPos = 0;
	m_sampleSumLeft = 0;
	m_sampleSumRight = 0;
	m_saveData.stereo = 0xFF;
	m_sampleTicks = 0;
	m_samples.clear();

	m_saveData.fmAddress = 0;
	m_saveData.fmControl = 0;
	OPLL_reset(m_opll);
}

void Sound::Tick() {
	// Canales de tono: la salida cambia de signo cada vez que el contador llega a 0
	for (int i = 0; i < 3; i++) {
		if (--m_saveData.counters[i] <= 0) {
			m_saveData.counters[i] = m_saveData.registers[i];
			m_saveData.outputs[i] = -m_saveData.outputs[i];
		}
	}

	// Ruido: el LFSR avanza cada dos recargas del contador
	if (--m_saveData.counters[3] <= 0) {
		int rate = m_saveData.registers[3] & 0x3;
		m_saveData.counters[3] = (rate == 3) ? m_saveData.registers[2] : (0x10 << rate);

		m_saveData.noiseToggle = !m_saveData.noiseToggle;
		if (m_saveData.noiseToggle) {
			int feedback;
			if (m_saveData.registers[3] & 0x4) {
				// Ruido blanco: bits 0 y 3
				int tapped = m_saveData.lfsr & 0x0009;
				feedback = (tapped == 0x0001) || (tapped == 0x0008);
			} else {
				// Ruido periódico
				feedback = m_saveData.lfsr & 1;
			}
			m_saveData.lfsr = (m_saveData.lfsr >> 1) | (feedback << 15);
		}
	}

	m_saveData.outputs[3] = (m_saveData.lfsr & 1) ? 1 : -1;

	int left = 0;
	int right = 0;
	for (int i = 0; i < 4; i++) {
		if (!SoundSDL::IsChannelEnabled(i))
			continue;

		// En el chip de Sega un periodo de 0 o 1 deja la salida fija a +1.
		// Los juegos lo usan para reproducir samples cambiando solo el volumen.
		int output = (i < 3 && m_saveData.registers[i] <= 1) ? 1 : m_saveData.outputs[i];
		int value = output * volumeTable.values[m_saveData.volumes[i]];

		// En Master System m_saveData.stereo es siempre 0xFF: los dos lados suenan igual
		if (m_saveData.stereo & (0x10 << i))
			left += value;
		if (m_saveData.stereo & (0x01 << i))
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
		int mode = s_fmEnabled ? (m_saveData.fmControl & 0x03) : 0;
		int fm = 0;
		if (s_fmEnabled) {
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
		if (m_fadeSamples < FADE_IN_SAMPLES) {
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
		m_saveData.pendingCycles = 0;
		return;
	}

	m_saveData.pendingCycles += (int) delta;
	while (m_saveData.pendingCycles >= CYCLES_PER_TICK) {
		m_saveData.pendingCycles -= CYCLES_PER_TICK;
		Tick();
	}
}

void Sound::WriteByte(Motherboard *cpu, uint8_t value) {
	// Primero genera el audio hasta el momento de la escritura
	Render(cpu->GetCycles());

	int channel;
	if (value & 0x80) {
		// Byte LATCH/DATA: %1cctdddd
		m_saveData.latchedChannel = (value >> 5) & 0x3;
		m_saveData.latchedVolume = (value & 0x10) != 0;
		channel = m_saveData.latchedChannel;

		if (m_saveData.latchedVolume)
			m_saveData.volumes[channel] = value & 0xF;
		else if (channel == 3) {
			m_saveData.registers[3] = value & 0x7;
			m_saveData.lfsr = 0x8000;
		} else
			m_saveData.registers[channel] = (m_saveData.registers[channel] & 0x3F0) | (value & 0xF);
	} else {
		// Byte DATA: %0-dddddd, se aplica al último registro seleccionado
		channel = m_saveData.latchedChannel;

		if (m_saveData.latchedVolume)
			m_saveData.volumes[channel] = value & 0xF;
		else if (channel == 3) {
			m_saveData.registers[3] = value & 0x7;
			m_saveData.lfsr = 0x8000;
		} else
			m_saveData.registers[channel] = (m_saveData.registers[channel] & 0x00F) | ((value & 0x3F) << 4);
	}
}

void Sound::WriteFMAddress(uint8_t value) {
	m_saveData.fmAddress = value & 0x3F;
}

void Sound::WriteFMData(Motherboard *cpu, uint8_t value) {
	// Primero genera el audio hasta el momento de la escritura
	Render(cpu->GetCycles());
	OPLL_writeReg(m_opll, m_saveData.fmAddress, value);
}

void Sound::WriteFMControl(Motherboard *cpu, uint8_t value) {
	Render(cpu->GetCycles());
	// Se guardan 3 bits: la detección los comprueba todos (el modo solo usa los 2 de abajo)
	m_saveData.fmControl = value & 0x07;
}

void Sound::WriteStereo(Motherboard *cpu, uint8_t value) {
	// Genera el audio hasta el momento de la escritura antes de cambiar el reparto
	Render(cpu->GetCycles());
	m_saveData.stereo = value;
}

// Estado: saveData, la estructura OPLL de emu2413 y, por cada uno de sus 18 operadores, el número de instrumento
// al que apunta (los punteros de OPLL no valen en otra ejecución)
#define OPLL_SLOTS 18

int Sound::GetSaveSize() {
	return sizeof(Sound::saveData) + sizeof(OPLL) + OPLL_SLOTS;
}

void Sound::SaveState(uint8_t *data) {
	memcpy(data, &m_saveData, sizeof(Sound::saveData));
	memcpy(data + sizeof(Sound::saveData), m_opll, sizeof(OPLL));

	uint8_t *patches = data + sizeof(Sound::saveData) + sizeof(OPLL);
	for (int i = 0; i < OPLL_SLOTS; i++) {
		ptrdiff_t index = m_opll->slot[i].patch - m_opll->patch;
		patches[i] = ((index >= 0) && (index < (ptrdiff_t) (sizeof(m_opll->patch) / sizeof(m_opll->patch[0])))) ? (uint8_t) index : 0;
	}
}

// cycle: contador de ciclos de la CPU ya restaurada. El audio se resincroniza ahí
// sin generar el salto de tiempo (hacia atrás o hacia delante) del estado cargado.
void Sound::LoadState(uint8_t *data, int64_t cycle) {
	memcpy(&m_saveData, data, sizeof(Sound::saveData));

	// Se rehacen los punteros: cada operador a su instrumento de este chip, el conversor de este chip, y la
	// tabla de onda (y lo que depende del instrumento) la recalcula emu2413 al pedirle una actualización
	OPLL_RateConv *conv = m_opll->conv;
	memcpy(m_opll, data + sizeof(Sound::saveData), sizeof(OPLL));
	m_opll->conv = conv;

	const uint8_t *patches = data + sizeof(Sound::saveData) + sizeof(OPLL);
	for (int i = 0; i < OPLL_SLOTS; i++) {
		m_opll->slot[i].patch = &m_opll->patch[patches[i] % (sizeof(m_opll->patch) / sizeof(m_opll->patch[0]))];
		m_opll->slot[i].update_requests = 0xFF; // UPDATE_ALL
	}

	m_lastCycle = cycle;
	m_tickPos = 0;
	m_sampleSumLeft = 0;
	m_sampleSumRight = 0;
	m_sampleTicks = 0;
	m_samples.clear();
}

void Sound::SetReverse(bool reverse) {
	if (m_reverse == reverse)
		return;

	m_reverse = reverse;
	m_fadeSamples = 0;
}

void Sound::EndFrame(Motherboard *cpu) {
	SoundSDL &soundSDL = SoundSDL::Instance();

	// El ajuste de ritmo corrige la pequeña diferencia entre el reloj emulado y el de la tarjeta de sonido
	double clock = cpu->GetVDP()->GetNTSC() ? CLOCK_NTSC : CLOCK_PAL;
	m_ticksPerSample = ((clock / CYCLES_PER_TICK) / SOUNDFREQ) * soundSDL.GetRateAdjust();

	Render(cpu->GetCycles());

	if (m_reverse) {
		// Pares estéreo en orden inverso
		size_t pairs = m_samples.size() / 2;
		for (size_t i = 0; i < pairs / 2; i++) {
			std::swap(m_samples[i * 2], m_samples[(pairs - 1 - i) * 2]);
			std::swap(m_samples[i * 2 + 1], m_samples[(pairs - 1 - i) * 2 + 1]);
		}
	}

	if (!m_samples.empty()) {
		soundSDL.Queue(this, m_samples.data(), (int) (m_samples.size() / 2));
		m_samples.clear();
	}
}
