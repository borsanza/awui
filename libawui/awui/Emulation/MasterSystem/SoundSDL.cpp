/**
 * awui/Emulation/MasterSystem/SoundSDL.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "SoundSDL.h"

#include <awui/Emulation/MasterSystem/Sound.h>

#include <algorithm>
#include <vector>

using namespace awui::Emulation::MasterSystem;

// Latencia objetivo en la cola (~46ms). Por debajo de un frame hay riesgo de cortes.
#define TARGET_QUEUED_SAMPLES 2048
// Si la cola crece por encima de esto (p.ej. tras un parón) se vacía para no acumular retraso
#define MAX_QUEUED_SAMPLES 8192
// Máxima corrección de ritmo (0.5%, inapreciable en el tono)
#define MAX_RATE_ADJUST 0.005

uint8_t SoundSDL::m_disabledChannels = 0x00;

SoundSDL::SoundSDL() {
	m_playing = NULL;
	m_audioDevice = 0;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		SDL_Log("[ERROR] SDL_InitSubSystem(SDL_INIT_AUDIO): %s", SDL_GetError());
		return;
	}

	SDL_AudioSpec desired;
	SDL_zero(desired);
	desired.freq = SOUNDFREQ;
	desired.format = AUDIO_S16SYS;
	desired.channels = 1;
	desired.samples = 512;
	desired.callback = NULL;

	// Sin cambios permitidos: si el dispositivo usa otro formato, SDL convierte
	m_audioDevice = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);

	if (m_audioDevice == 0) {
		SDL_Log("[ERROR] SDL_OpenAudioDevice: %s", SDL_GetError());
		return;
	}

	SDL_PauseAudioDevice(m_audioDevice, 0);
}

SoundSDL::~SoundSDL() {
	if (m_audioDevice != 0)
		SDL_CloseAudioDevice(m_audioDevice);
}

SoundSDL &SoundSDL::Instance() {
	static SoundSDL instance;
	return instance;
}

int SoundSDL::GetQueuedSamples() const {
	if (m_audioDevice == 0)
		return 0;

	return (int) (SDL_GetQueuedAudioSize(m_audioDevice) / sizeof(int16_t));
}

void SoundSDL::SetPlayingSound(Sound *sound) {
	if (m_playing == sound)
		return;

	m_playing = sound;

	if (m_audioDevice != 0)
		SDL_ClearQueuedAudio(m_audioDevice);
}

void SoundSDL::Queue(Sound *sound, const int16_t *samples, int count) {
	if (!IsPlaying(sound) || count <= 0)
		return;

	int queued = GetQueuedSamples();

	if (queued > MAX_QUEUED_SAMPLES) {
		SDL_ClearQueuedAudio(m_audioDevice);
		queued = 0;
	}

	// Al arrancar o tras quedarse sin datos, añade silencio para recuperar el margen
	if (queued == 0) {
		std::vector<int16_t> silence(TARGET_QUEUED_SAMPLES / 2, 0);
		SDL_QueueAudio(m_audioDevice, silence.data(), (Uint32) (silence.size() * sizeof(int16_t)));
	}

	SDL_QueueAudio(m_audioDevice, samples, (Uint32) (count * sizeof(int16_t)));
}

// > 1 cuando sobran muestras en la cola (hay que generar menos), < 1 cuando faltan
double SoundSDL::GetRateAdjust() const {
	if (m_audioDevice == 0)
		return 1.0;

	double error = double(GetQueuedSamples() - TARGET_QUEUED_SAMPLES) / TARGET_QUEUED_SAMPLES;
	error = std::clamp(error, -1.0, 1.0);

	return 1.0 + (error * MAX_RATE_ADJUST);
}

void SoundSDL::ToggleChannel(int channel) {
	m_disabledChannels ^= 1 << channel;
}
