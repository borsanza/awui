/**
 * awui/Emulation/Common/AudioOutput.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "AudioOutput.h"

#include <awui/Emulation/Common/AudioSettings.h>

#include <algorithm>
#include <vector>

using namespace awui::Emulation::Common;

// Latencia objetivo en la cola, en frames estéreo (~46ms). Por debajo de un frame hay riesgo de cortes.
#define TARGET_QUEUED_FRAMES 2048
// Si la cola crece por encima de esto (p.ej. tras un parón) se vacía para no acumular retraso
#define MAX_QUEUED_FRAMES 8192
// Máxima corrección de ritmo (0.5%, inapreciable en el tono)
#define MAX_RATE_ADJUST 0.005

AudioOutput::AudioOutput() {
	m_playing = nullptr;
	m_audioDevice = 0;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		SDL_Log("[ERROR] SDL_InitSubSystem(SDL_INIT_AUDIO): %s", SDL_GetError());
		return;
	}

	SDL_AudioSpec desired;
	SDL_zero(desired);
	desired.freq = Frequency;
	desired.format = AUDIO_S16SYS;
	desired.channels = 2;
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

AudioOutput::~AudioOutput() {
	if (m_audioDevice != 0)
		SDL_CloseAudioDevice(m_audioDevice);
}

AudioOutput &AudioOutput::Instance() {
	static AudioOutput instance;
	return instance;
}

int AudioOutput::GetQueuedFrames() const {
	if (m_audioDevice == 0)
		return 0;

	return (int) (SDL_GetQueuedAudioSize(m_audioDevice) / (2 * sizeof(int16_t)));
}

void AudioOutput::SetPlaying(const void *source) {
	if (m_playing == source)
		return;

	m_playing = source;

	if (m_audioDevice != 0)
		SDL_ClearQueuedAudio(m_audioDevice);
}

void AudioOutput::Queue(const void *source, const int16_t *samples, int frames, int channels) {
	if (!IsPlaying(source) || (frames <= 0))
		return;

	int queued = GetQueuedFrames();

	if (queued > MAX_QUEUED_FRAMES) {
		SDL_ClearQueuedAudio(m_audioDevice);
		queued = 0;
	}

	// Al arrancar o tras quedarse sin datos, añade silencio (medio margen) para recuperarlo
	if (queued == 0) {
		std::vector<int16_t> silence(TARGET_QUEUED_FRAMES, 0);
		SDL_QueueAudio(m_audioDevice, silence.data(), (Uint32) (silence.size() * sizeof(int16_t)));
	}

	// Volumen: se aplica al encolar (con el sonido desactivado se encola silencio para no perder el ritmo)
	int gain = AudioSettings::GetGain();
	if ((channels == 2) && (gain == 100)) {
		SDL_QueueAudio(m_audioDevice, samples, (Uint32) (frames * 2 * sizeof(int16_t)));
		return;
	}

	std::vector<int16_t> stereo(frames * 2);
	for (int i = 0; i < frames; i++) {
		int16_t left = (channels == 2) ? samples[i * 2] : samples[i];
		int16_t right = (channels == 2) ? samples[i * 2 + 1] : samples[i];
		stereo[i * 2] = (int16_t) ((left * gain) / 100);
		stereo[i * 2 + 1] = (int16_t) ((right * gain) / 100);
	}

	SDL_QueueAudio(m_audioDevice, stereo.data(), (Uint32) (stereo.size() * sizeof(int16_t)));
}

double AudioOutput::GetRateAdjust() const {
	if (m_audioDevice == 0)
		return 1.0;

	double error = double(GetQueuedFrames() - TARGET_QUEUED_FRAMES) / TARGET_QUEUED_FRAMES;
	error = std::clamp(error, -1.0, 1.0);

	return 1.0 + (error * MAX_RATE_ADJUST);
}

bool AudioOutput::IsQueueFull() const {
	return GetQueuedFrames() >= TARGET_QUEUED_FRAMES;
}
